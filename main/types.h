enum MESSAGE_OP{
    CONNECTION_ATTEMPT,
    SERVER_CONNECTION_STABLISHED,
    SYSTEM_PARAM_CHANGE,
    SYSTEM_STARTUP,
    SYSTEM_STATUS,
    SYSTEM_INTR,
    SYSTEM_SHUTDOWN
};

typedef struct system_params_struct{
    int input_valve_flow_speed;
    int middle_valve_flow_speed;
    int output_valve_flow_speed;
    int water_boiling_rate;
    int sensor_reading_timer;
    int water_tank_water_max_level;
    int water_tank_water_min_level;
    int boiling_tank_water_max_level;
    int boiling_tank_water_min_level;
} system_params_t;

typedef struct sensor_readings_struct{
    int max_sensor_tank1;
    int min_sensor_tank1;
    int water_level_tank1;
    int temp_water_tank2;
    int max_sensor_tank2;
    int min_sensor_tank2;
    int water_level_tank2;
    int input_valve_status;
    int middle_valve_status;
    int output_valve_status;
    int resistance_status;
    int water_is_boiled;
} sensor_readings_t;

typedef struct data_packet_struct{
    int message_type;
    system_params_t system_settings;
    sensor_readings_t sensor_readings;
} data_packet_t;