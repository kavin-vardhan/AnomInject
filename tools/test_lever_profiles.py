from pathlib import Path
import unittest
from unittest.mock import patch
import lever_audit as audit
import lever_profiles as profiles

ROOT=Path(__file__).resolve().parent.parent

class Profiles(unittest.TestCase):
    def test_declarations(self):
        self.assertEqual(len(profiles.declaration_proof(ROOT)),5)

    def test_supported_target_macros(self):
        for engine in ('425','51'):
            configs=profiles.configurations(engine)
            for name,values in configs.items():
                shipping=name=='Shipping'
                self.assertEqual(values['ANOMALY_CAPTURE'],int(not shipping))
                self.assertEqual(values['ANOMALY_BENCH'],int(engine=='51' and not shipping))
                self.assertEqual(values['WITH_EDITOR'],int(name=='Editor'))
        for engine in (None,'50','57',''):
            with self.assertRaises(ValueError):profiles.configurations(engine)

    def test_unknown_condition_is_conservative(self):
        for condition in ('UNDECLARED_FEATURE','defined(UNDECLARED_FEATURE)','1 && UNDECLARED_FEATURE'):
            self.assertIsNone(audit.eval_pp(condition,profiles.configurations('425')['Development']))
        states=audit.line_activity(['#if UNDECLARED_FEATURE','a','#else','b','#endif'],{})
        self.assertIsNone(states[1]);self.assertIsNone(states[3])

    def test_active_binary_rule_mutants(self):
        for engine in ('425','51'):
            configs=profiles.configurations(engine)
            names=set();registrations=set()
            for path in (ROOT/'Source').rglob('*'):
                if path.suffix not in ('.cpp','.h'):continue
                source=audit.Source(str(ROOT),str(path.relative_to(ROOT)),configs)
                for start,end,value in source.lits:
                    if source.live(start,'Development') is not True:continue
                    found=audit.NAME_RX.findall(value)
                    names.update(found)
                    if audit.REG_RX.search(source.prefix(start)) and found and value==found[0]:registrations.add(value)
            self.assertTrue(registrations)
            self.assertEqual(audit.binary_rules(names,names,registrations,[]),[])
            removed=sorted(registrations)[0]
            self.assertIn(('B2','source registration absent from binary: '+removed),audit.binary_rules(names-{removed},names,registrations,[]))
            self.assertIn(('B1','binary carries IAI.Bench.ExtraMutant'),audit.binary_rules(names|{'IAI.Bench.ExtraMutant'},names,registrations,[]))
            self.assertEqual(audit.binary_rules(names,names,registrations,['IAI bench levers: ENABLED'])[0][0],'B3')

    def test_empty_inventory_refused(self):
        for found,names,registrations in ((set(),{'x'},{'x'}),({'x'},set(),{'x'}),({'x'},{'x'},set())):
            self.assertTrue(any(rule=='B2' for rule,_ in audit.binary_rules(found,names,registrations,[])))

    def test_wrong_profile_refused_before_artifact(self):
        for payload in ({'engine':'51','configuration':'Development'},{'engine':'425','configuration':'Editor'}):
            with patch.object(profiles,'checked_json',return_value=payload):
                with self.assertRaisesRegex(ValueError,'Wrong binary profile'):
                    profiles.validate_provenance(ROOT,'425','Development',['synthetic.dll'],'synthetic.json','synthetic-pin')

    def test_missing_pin_refused(self):
        with self.assertRaisesRegex(ValueError,'Missing or wrong receipt pin'):
            profiles.checked_json('nonexistent.json',None)

if __name__=='__main__':unittest.main()
