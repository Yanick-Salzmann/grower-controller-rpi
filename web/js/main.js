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

$(() => {
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

    /*setInterval(() => {
        $.ajax('/fans')
            .done((data) => {
                $('#cooler-fan-rpm').text("Current RPM: " + data.cooler);
            })
    }, 1000);*/
});