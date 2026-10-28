from pathlib import Path
import os
from owner_attempts import Guard as BaseGuard, create_json, stamp
from fixture_policy import canonical, CITATION, RULE


class Guard(BaseGuard):
    def __init__(self, config, recipe):
        super().__init__(dict(config['recipes'][recipe], job_template=config['job_template'],
                              dry_run_only=config['dry_run_only']))
        self.recipe=recipe

    def check_unused_launch(self):
        if (self.state/'launched.json').exists() or (self.state/'launch-reserved.json').exists():
            raise RuntimeError(self.recipe+': the one authorized session is spent or reserved; no retry is permitted.')

    def claim(self):
        self.check_unused_launch()
        for slot in self.config['attempt_slots']:
            root=canonical(slot['job_root']);temp=canonical(slot['temp_root'])
            if root.exists() or temp.exists():
                continue
            try:
                root.mkdir(parents=True,exist_ok=False)
            except FileExistsError:
                continue
            scope=dict(self.config['job_template'],job_root=str(root),temp_root=str(temp),started_utc=stamp())
            create_json(root/'claim.json',dict(recipe=self.recipe,slot=slot['number'],claimed_utc=stamp(),pid=os.getpid(),dry_run_only=self.config['dry_run_only']))
            create_json(root/'job-config.json',scope)
            return slot,root/'job-config.json'
        raise RuntimeError(self.recipe+': all three prelaunch attempts are used; no editor was started.')

    def mark_launched(self,slot,pid):
        create_json(self.state/'launched.json',dict(recipe=self.recipe,slot=slot['number'],editor_pid=pid,
            launched_utc=stamp(),authorization_citation=CITATION,policy_citation=RULE))


class GlobalGuard(BaseGuard):
    def __init__(self,config):
        super().__init__(dict(state_root=config['global_state']))
