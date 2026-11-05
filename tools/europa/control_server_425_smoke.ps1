param(
    [Parameter(Mandatory=$true)][string]$LogPath,
    [Parameter(Mandatory=$true)][string]$Output,
    [Parameter(Mandatory=$true)][string]$CaptureBase,
    [ValidateSet('Smoke','Full','EarlyFrames')][string]$Mode='Smoke',
    [int]$Port=8077
)
$ErrorActionPreference='Stop'
$rows=New-Object System.Collections.Generic.List[object]
$clients=New-Object System.Collections.Generic.List[object]
$credential=$null
function Read-CaptureLog([string]$Path) {
    $stream=[IO.FileStream]::new($Path,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::ReadWrite)
    $reader=$null
    try {
        $reader=[IO.StreamReader]::new($stream)
        return $reader.ReadToEnd()
    } finally {
        if ($null -ne $reader) { $reader.Dispose() } else { $stream.Dispose() }
    }
}
function Record($kind,$value) { $rows.Add(@{utc=[DateTime]::UtcNow.ToString('o');kind=$kind;value=$value}) }
function Need($ok,$name) { if (-not $ok) { throw ('Gate failed: '+$name) }; Record 'check' $name }
function Connect-Peer {
    $ws=New-Object System.Net.WebSockets.ClientWebSocket
    $clients.Add($ws)
    $cancel=New-Object System.Threading.CancellationTokenSource
    $cancel.CancelAfter(10000)
    try { $ws.ConnectAsync([Uri]('ws://127.0.0.1:'+$Port+'/'),$cancel.Token).GetAwaiter().GetResult() } finally { $cancel.Dispose() }
    return $ws
}
function Send-Packet($ws,$packet,[switch]$Fragment) {
    $bytes=[Text.Encoding]::UTF8.GetBytes(($packet | ConvertTo-Json -Depth 12 -Compress))
    $cancel=New-Object System.Threading.CancellationTokenSource
    $cancel.CancelAfter(10000)
    try {
        if ($Fragment) {
            $cut=[Math]::Max(1,[int]($bytes.Length/2))
            $ws.SendAsync([ArraySegment[byte]]::new($bytes,0,$cut),[Net.WebSockets.WebSocketMessageType]::Text,$false,$cancel.Token).GetAwaiter().GetResult()
            $ws.SendAsync([ArraySegment[byte]]::new($bytes,$cut,$bytes.Length-$cut),[Net.WebSockets.WebSocketMessageType]::Text,$true,$cancel.Token).GetAwaiter().GetResult()
        } else { $ws.SendAsync([ArraySegment[byte]]::new($bytes),[Net.WebSockets.WebSocketMessageType]::Text,$true,$cancel.Token).GetAwaiter().GetResult() }
    } finally { $cancel.Dispose() }
    Record 'sent' $packet.type
}
function Receive-Type($ws,$wanted,[int]$Seconds=15) {
    $until=[DateTime]::UtcNow.AddSeconds($Seconds)
    do {
        $stream=New-Object IO.MemoryStream
        $buffer=New-Object byte[] 65536
        $cancel=New-Object Threading.CancellationTokenSource
        $cancel.CancelAfter([Math]::Max(1,[int]($until-[DateTime]::UtcNow).TotalMilliseconds))
        try {
            do {
                $part=$ws.ReceiveAsync([ArraySegment[byte]]::new($buffer),$cancel.Token).GetAwaiter().GetResult()
                if ($part.MessageType -ne [Net.WebSockets.WebSocketMessageType]::Binary) { throw 'Expected raw binary JSON' }
                $stream.Write($buffer,0,$part.Count)
                if ($stream.Length -gt 16777216) { throw 'Inbound test ceiling exceeded' }
            } while (-not $part.EndOfMessage)
            $bytes=$stream.ToArray()
            if ($bytes.Length -eq 0 -or $bytes[0] -ne 123) { throw 'Non-JSON prefix on server message' }
            $packet=[Text.Encoding]::UTF8.GetString($bytes) | ConvertFrom-Json
        } finally { $cancel.Dispose();$stream.Dispose() }
        Record 'received' $packet.type
        if ($packet.type -eq $wanted) { return $packet }
        if ($packet.type -eq 'error') { throw ('Protocol refusal: '+$packet.code) }
    } while ([DateTime]::UtcNow -lt $until)
    throw ('Timed out awaiting '+$wanted)
}
function Status($ws) { Send-Packet $ws @{type='capture_status'};return Receive-Type $ws 'capture_status' }
function Start-Capture($ws) {
    Send-Packet $ws @{type='capture_start';dir=$CaptureBase;format='png';seed=4242;maxFrames=2352}
    $ack=Receive-Type $ws 'ack';Need ($ack.re -eq 'capture_start') 'start acknowledged'
    $state=Status $ws;Need ($state.running -and $state.seed -eq 4242 -and $state.maxFrames -eq 2352) 'authoritative warmup status'
    Record 'start' $state
    return $state
}
$verdict='FAIL';$failure=$null
try {
    if (Test-Path -LiteralPath $Output) { throw 'Fresh output file required' }
    $log=Read-CaptureLog $LogPath
    $found=[regex]::Matches($log,'Control server token:\s*([^\s]+)')
    if ($found.Count -eq 0) { throw 'Listener announcement absent' }
    $credential=$found[$found.Count-1].Groups[1].Value;$log=$null;$found=$null
    $bad=Connect-Peer
    Send-Packet $bad @{type='hello';token=[Guid]::NewGuid().ToString('N');v=1}
    $refusal=Receive-Type $bad 'error';Need ($refusal.code -eq 'bad_token') 'bad token refused';$bad.Dispose()
    $ws=Connect-Peer
    Send-Packet $ws @{type='hello';token=$credential;v=1} -Fragment
    $welcome=Receive-Type $ws 'welcome';Need ($welcome.v -eq 1) 'fragmented hello authenticated'
    Send-Packet $ws @{type='subscribe';channels=@('snapshot','frames');snapshotHz=5;frameHz=6}
    $null=Receive-Type $ws 'ack'
    Send-Packet $ws @{type='list_anomalies'}
    $catalog=Receive-Type $ws 'catalog';$ids=@($catalog.entries | ForEach-Object {$_.id})
    foreach ($id in @('blinking','lod_corruption','missing_texture','corrupted_texture')) { Need ($ids -contains $id) ('catalog '+$id) }
    Record 'catalog_ids' $ids
    $null=Receive-Type $ws 'snapshot'
    $initial=Start-Capture $ws
    Send-Packet $ws @{type='auto_fire_once'}
    $refusal=Receive-Type $ws 'error';Need ($refusal.code -eq 'capture_active') 'mutator refused during warmup'
    if ($Mode -eq 'Smoke') {
        Send-Packet $ws @{type='capture_stop'}
        $stopped=Receive-Type $ws 'capture_stopped';Need (-not $stopped.running -and $stopped.frames -eq 0) 'warmup stopped with zero frames'
        Record 'stop' $stopped
        Start-Sleep -Seconds 14
        $state=Status $ws;Need (-not $state.running -and $state.frames -eq 0) 'cancelled request never started later'
        $next=Start-Capture $ws;Need ($next.sessionId -ne $initial.sessionId) 'fresh second session'
        Send-Packet $ws @{type='capture_stop'}
        $stopped=Receive-Type $ws 'capture_stopped';Need (-not $stopped.running -and $stopped.frames -eq 0) 'second warmup stop'
        Record 'second_stop' $stopped
    } else {
        $until=[DateTime]::UtcNow.AddSeconds(900)
        do {
            Start-Sleep -Seconds 1;$state=Status $ws
            if ($Mode -eq 'EarlyFrames' -and $state.frames -ge 30) {
                Send-Packet $ws @{type='capture_stop'};$state=Receive-Type $ws 'capture_stopped';break
            }
        } while ($state.running -and [DateTime]::UtcNow -lt $until)
        Need (-not $state.running) 'capture finished or stopped'
        if ($Mode -eq 'Full') { Need ($state.frames -eq 2352) '2352 completed frames' }
        else { Need ($state.frames -gt 0 -and $state.frames -lt 2352) 'early stop stays partial' }
        Record 'final' $state
    }
    $ws.Dispose();$ws=Connect-Peer
    Send-Packet $ws @{type='hello';token=$credential;v=1};$null=Receive-Type $ws 'welcome'
    $state=Status $ws;Need (-not $state.running) 'reconnect recovers final state'
    Record 'reconnected' $state
    $verdict='PASS'
} catch { $failure=$_.Exception.GetType().Name+': '+$_.Exception.Message }
finally {
    foreach ($client in $clients) { $client.Dispose() }
    $credential=$null
    $result=@{result=$verdict;mode=$Mode;log_referenced_in_place=$LogPath;tokens='<masked>';failure=$failure;records=$rows}
    [IO.File]::WriteAllText($Output,($result | ConvertTo-Json -Depth 15),(New-Object Text.UTF8Encoding $false))
}
Write-Output ($verdict+' protocol '+$Mode)
if ($verdict -ne 'PASS') { exit 1 }
