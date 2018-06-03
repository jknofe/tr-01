// http://jsbeautifier.org

// de-obsfuscated code from foxed.ca


$(document)['ready'](function() {
    $('#pressure_select')['change'](function() {
        if ($('#result_1')['val']()) {
            _0x3850x6()
        }
    });
    $('#convert_button')['click'](function() {
        _0x3850x6()
    });
    $('#print_button')['click'](function() {
        _0x3850x3('resultArea')
    });
    $('#copy_button')['click'](function() {
        _0x3850x1('resultArea')
    });

    function _0x3850x1(_0x3850x2) {
        let;
        t = document['createElement']('textarea');
        t['id'] = 't';
        t['style']['height'] = 0;
        document['body']['appendChild'](t);
        t['value'] = document['getElementById'](_0x3850x2)['innerText'];
        let;
        selector = document['querySelector']('#t');
        selector['select']();
        document['execCommand']('copy');
        document['body']['removeChild'](t)
    }

    function _0x3850x3(_0x3850x2) {
        var _0x3850x4 = document['getElementById'](_0x3850x2);
        var _0x3850x5 = window['open']('', '', 'width=900,height=650');
        _0x3850x5['document']['write'](_0x3850x4['innerHTML']);
        _0x3850x5['document']['close']();
        _0x3850x5['focus']();
        _0x3850x5['print']();
        _0x3850x5['close']()
    }

    function _0x3850x6() {
        var face1_psi = $('#face_1')['val']();
        var face2_psi = $('#face_2')['val']();
        var face3_psi = $('#face_3')['val']();
        var rpm = $('#rpm')['val']();
        var altitude_val = $('#altitude')['val']();
        var engineType = $('#engine_select')['val']();
        var pressureSelection = $('#pressure_select')['val']();
        var face1_result;
        var face2_result;
        var face3_result;
        var pressure_conv_from_psi;
        var altitude_correction;
        var sideport_correction;
        var _0x3850x14;
        var engine_type_correction;
        if ($('#altitude_unit')['val']() == 'Meters') {
            altitude_val = altitude_val * 3.2808399
        };
        altitude_correction = Math['exp'](-0.0000435184 * altitude_val);
        altitude_correction = (5.983051463 * altitude_correction) / 5.9;
        altitude_correction = 1 / altitude_correction;
        if (engineType == '13B Renesis 04-11') {
            sideport_correction = (0.019955 * rpm + 3.493182) / 8.5;
            sideport_correction = 1 / sideport_correction
        } else {
            sideport_correction = (0.017455 * rpm + 1.609091) / 5.9;
            sideport_correction = 1 / sideport_correction
        };
        switch (engineType) {
            case ('12A 76-82'):
                ;
            case ('12A 83-85'):
                _0x3850x14 = 9.4;
                engine_type_correction = 1.042;
                break;
            case ('13B 74-78'):
                _0x3850x14 = 9.2;
                engine_type_correction = 1.035;
                break;
            case ('13B 84-85'):
                ;
            case ('13B NA 86-88'):
                _0x3850x14 = 9.4;
                engine_type_correction = 1.036;
                break;
            case ('13B Turbo 86-88'):
                _0x3850x14 = 8.5;
                engine_type_correction = 1.033;
                break;
            case ('13B NA 89-92'):
                _0x3850x14 = 9.7;
                engine_type_correction = 1.038;
                break;
            case ('13B Turbo 89-92'):
                ;
            case ('13B REW 93-95'):
                _0x3850x14 = 9.0;
                engine_type_correction = 1.034;
                break;
            case ('13B Renesis 04-11'):
                _0x3850x14 = 10.0;
                engine_type_correction = 1.039;
                break;
            default:
                alert('There has been an error.')
        };
        switch (pressureSelection) {
            case ('PSI'):
                pressure_conv_from_psi = 1;
                break;
            case ('kPa'):
                pressure_conv_from_psi = 6.894757293178;
                break;
            case ('bar'):
                pressure_conv_from_psi = 0.06894757293;
                break;
            case ('kg/cm2'):
                pressure_conv_from_psi = 0.07030695796402;
                break;
            default:
                alert('There has been an error.')
        };
        face1_result = face1_psi * engine_type_correction * altitude_correction * sideport_correction * pressure_conv_from_psi;
        face2_result = face2_psi * engine_type_correction * altitude_correction * sideport_correction * pressure_conv_from_psi;
        face3_result = face3_psi * engine_type_correction * altitude_correction * sideport_correction * pressure_conv_from_psi;
        $('#result_1')['val'](face1_result['toFixed'](2));
        $('#result_2')['val'](face2_result['toFixed'](2));
        $('#result_3')['val'](face3_result['toFixed'](2));
        $('#compression_ratio')['val'](_0x3850x14);
        var _0x3850x16 = document['getElementById']('resultArea');
        _0x3850x16['innerHTML'] = 'Foxed.ca Rotary Compression Calculator Results<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + '------------------------------------------------------------<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Initial Data:<br/><br/>';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Face 1: ' + face1_psi + ' PSI<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Face 2: ' + face2_psi + ' PSI<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Face 3: ' + face3_psi + ' PSI<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Cranking RPM: ' + rpm + '<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Altitude: ' + $('#altitude')['val']() + ' ' + $('#altitude_unit')['val']() + '<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Engine: ' + engineType + '<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Compression Ratio: ' + _0x3850x14 + '<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + '------------------------------------------------------------<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Corrected Values:<br/><br/>';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Face 1: ' + face1_result['toFixed'](2) + ' ' + pressureSelection + '<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Face 2: ' + face2_result['toFixed'](2) + ' ' + pressureSelection + '<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + 'Face 3: ' + face3_result['toFixed'](2) + ' ' + pressureSelection + '<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + '------------------------------------------------------------<br />';
        _0x3850x16['innerHTML'] = _0x3850x16['innerHTML'] + '&copy; Copyright 2016 <a href="http://foxed.ca">Foxed.ca</a> Inc. All Rights Reserved.'
    }
})