let psu_state = false;

const cooler_fan = 1;
const humidifier_fan = 2;
const intake_fan = 4;
const all_fans = cooler_fan | humidifier_fan | intake_fan;

function fetch_psu_state() {
    $.ajax('/psu', {
        type: 'GET',
    }).done((data) => {
        const elem = $('#psu-switch-icon');
        if (data.enabled) {
            psu_state = true;
            elem.removeClass('power-off').addClass('power-on');
        } else {
            psu_state = false;
            elem.removeClass('power-on').addClass('power-off');
        }
    })
}

function fetch_power_current(type, element, voltage) {
    $.ajax('/current/' + type, {type: 'GET'})
        .done(data => {
            const current = parseFloat(data);
            $(element).text((current * voltage).toFixed(2) + "W");
        })
    setTimeout(() => fetch_power_current(type, element, voltage), 500);
}

function fetch_power_state(type, element) {
    $.ajax('/state/' + type)
        .done(data => {
            const enabled = data === "1";
            $(element).prop('forced', true);
            $(element).prop('checked', enabled ? true : null);
            $(element).prop('forced', null);
        })
    setTimeout(() => fetch_power_state(type, element), 10);
}

function set_relay_checkbox(element, type) {
    $(element).change(function () {
        if ($(element).prop('forced')) {
            return;
        }

        $(element).prop('force', true);
        $.ajax('/state/' + type, {
                type: 'POST',
                data: JSON.stringify({
                    enabled: $(element).is(':checked')
                }),
                contentType: 'application/json; charset=utf-8',
                dataType: 'json'
            }
        ).done(() => {
            $(element).prop('force', null);
        })
    })
}

$(() => {
    fetch_power_current("lamp", "#lamp-power-indicator", 230);
    fetch_power_current("rpi", "#rpi-power-indicator", 5);

    fetch_power_state("flower", '#flowering-lamp-check');
    fetch_power_state("germination", "#germination-lamp-check");
    fetch_power_state('illumination', '#illumination-lamp-check');

    set_relay_checkbox("#flowering-lamp-check", 'flower');
    set_relay_checkbox("#germination-lamp-check", 'germination');
    set_relay_checkbox('#illumination-lamp-check', 'illumination');

    $(document).on('input', '#cooler-fan-speed', function () {
        $.ajax('/fans', {
            type: 'PUT',
            data: JSON.stringify({
                fans: 7,
                speed: parseInt($(this).val())
            }),
            contentType: 'application/json; charset=utf-8',
            dataType: 'json'
        })
    });

    fetch_psu_state();

    $('#psu-switch-icon').click(() => {
        $.ajax("/psu", {
            type: 'PUT',
            data: JSON.stringify({
                enabled: !psu_state
            }),
            contentType: 'application/json; charset=utf-8',
            dataType: 'json'
        }).done(() => {
            fetch_psu_state();
        })
    });
});