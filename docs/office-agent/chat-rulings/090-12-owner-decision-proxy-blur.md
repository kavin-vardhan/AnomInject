# Owner decision — 2026-10-02 ~11:00 IST: blurry textures, option 2

Context: office readings (second office host, PIE, m53 88801bd) show stuck_low_mip refused on shared textures (shared_world 196; 164 with ≥ 9 users, max 6,155). Per the pre-declared 084-11 §1.1 reading → fallback 2.

**Owner's choice: 2.**
- Monday ships stuck_low_mip **available but OFF by default**, with a readme limitation (fires rarely on content with heavily shared textures).
- Code builds the **per-target private-copy ("proxy") blur** over the weekend on a separate branch (`feat/m52-proxy-blur`, brief 090-12).
- It goes into Monday's delivery **only if fully proven at home (PIE + staged) and on an office host by Sunday noon IST**; otherwise it ships in the follow-up drop. No risk to the rest of the delivery.
