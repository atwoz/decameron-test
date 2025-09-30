#include "decameron.h"

static decameron_context_t _context;


bool _decameron_is_present(void)
{
    // check if is night
    if (_context.is_night) {
        if (_context.last_presence < _context.conditions.presence_timeout_night) {
            _context.was_present = true;
            return true;
        }
    } else {
        if (_context.last_presence < _context.conditions.presence_timeout_day) {
            _context.was_present = true;
            return true;
        }
    }

    return false;
}

/*
    Analiza si hay un incremento positivo en la humedad y la temperatura
    agarrando los ultimos 30 minutos y si detecta un delta positivo de  20% de humedad y de 2 grados de temperatura entonces devuelve true
*/
bool _decameron_check_positive_delta_in_humidity_and_temp(void)
{
    uint32_t i;
    int32_t index;
    int32_t index_prev;
    float delta_humidity = 0;
    float delta_temp = 0;


    for (i = 0; i < 30; i++) {
        index = _context.measurement_index - i;
        if (index < 0) {
            index = MAX_MEASUREMENTS - 1;
        }

        index_prev = index - 1;
        if (index_prev < 0) {
            index_prev = MAX_MEASUREMENTS - 1;
        }

        delta_humidity += _context.measurement[index].humidity - _context.measurement[index_prev].humidity;
        delta_temp += _context.measurement[index].temperature - _context.measurement[index_prev].temperature;
        if (delta_humidity >= 30 || delta_temp >= 3) {
            return true;
        }
    }

    return false;
}

/*
    Si hay el timeout de comando esta activo no hacemos nada
    Si la presnecia es mayor al timeout, enviamos temperatura alta
    Si la temperatura es mayor a la maxima, enviamos temperatura alta
    Si la temperatura es menor a la minima, enviamos temperatura baja
    Si la humedad es mayor a 90%, enviamos 1 grado menor a la temperatura baja, siempre limitado a la temperatura baja

    cada que enviamos un comando reiniciamos en cero el last_cmd 


    regreasmos false cuando no hay ningun comando que enviar
*/
bool _decameron_process(dec_ac_state_t *ac_state) {
    int32_t current_index = _context.measurement_index - 1;
    if (current_index < 0) {
        current_index = MAX_MEASUREMENTS - 1;
    }
    if (_context.last_cmd < _context.conditions.cmd_timeout_day) {
        return false;
    }
    if (_context.last_cmd < _context.conditions.cmd_timeout_night) {
        return false;
    }

    if (_decameron_is_present() == false) {
        if (_context.was_present == true)
        {
            // acaba de estar ausente
            // Enviamos comando de temperatura alta
            _context.was_present = false;
            ac_state->temp = _context.cmd.cmd_temp_high;
            ac_state->fan = _context.cmd.cmd_fan_high;
            _context.last_cmd = 0;
            return true;
        }
    }

    // Si la temperatura es mayor a la maxima, enviamos temperatura alta
    if (_context.measurement[current_index].temperature >= _context.conditions.max_temperature) {
        ac_state->temp = _context.cmd.cmd_temp_high;
        ac_state->fan = _context.cmd.cmd_fan_high;
        _context.last_cmd = 0;
        return true;
    }

    if (_context.measurement[current_index].temperature <= _context.conditions.min_temperature) {
        ac_state->temp = _context.cmd.cmd_temp_low;
        ac_state->fan = _context.cmd.cmd_fan_low;
        _context.last_cmd = 0;
        return true;
    }

    if (_context.measurement[current_index].humidity >= _context.conditions.max_humidity) {
        ac_state->temp = _context.measurement[current_index].temperature - 1;
        if (ac_state->temp > _context.cmd.cmd_temp_high) {
            ac_state->temp = _context.cmd.cmd_temp_high;
        }
        if (ac_state->temp < _context.cmd.cmd_temp_low) {
            ac_state->temp = _context.cmd.cmd_temp_low;
        }
        ac_state->fan = _context.cmd.cmd_fan_low;
        _context.last_cmd = 0;
        return true;
    }

    if (_decameron_check_positive_delta_in_humidity_and_temp()) {
        ac_state->temp = _context.measurement[current_index].temperature - 1;
        if (ac_state->temp > _context.cmd.cmd_temp_high) {
            ac_state->temp = _context.cmd.cmd_temp_high;
        }
        if (ac_state->temp < _context.cmd.cmd_temp_low) {
            ac_state->temp = _context.cmd.cmd_temp_low;
        }
        ac_state->fan = _context.cmd.cmd_fan_low;
        _context.last_cmd = 0;
        return true;
    }
    
    return false;
}


// PUBLIC

void DECAMERON_init(void) {
    _context.measurement_index = 0;
    _context.minute_count = 0;
    _context.conditions.min_temperature = 22.5;
    _context.conditions.max_temperature = 25.5;
    _context.conditions.max_humidity = 90;
    _context.conditions.presence_timeout_day = 30;
    _context.conditions.presence_timeout_night = 240;
    _context.conditions.cmd_timeout_day = 30;
    _context.conditions.cmd_timeout_night = 60;
    _context.cmd.cmd_temp_high = 25;
    _context.cmd.cmd_fan_high = 1;
    _context.cmd.cmd_temp_low = 23;
    _context.cmd.cmd_fan_low = 3;
    _context.last_cmd = 0;
}

bool DECAMERON_feed(dec_measurement_t *measurement, dec_ac_state_t *ac_state) {
    // add to circular buffer
    _context.measurement[_context.measurement_index] = *measurement;
    _context.measurement_index++;
    if (_context.measurement_index >= MAX_MEASUREMENTS) {
        _context.measurement_index = 0;
    }
    _context.minute_count++;
    _context.last_cmd++;

    return _decameron_process(ac_state);
}
