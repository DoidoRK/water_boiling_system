enum MESSAGE_OP{
    CONNECTION_ATTEMPT,
    SERVER_CONNECTION_STABLISHED,
    SYSTEM_PARAM_CHANGE,
    SYSTEM_STARTUP,
    SYSTEM_INTR,
    SYSTEM_SHUTDOWN
};

typedef struct system_settings_struct{
    int input_valve_flow_speed;
    int middle_valve_flow_speed;
    int output_valve_flow_speed;
    int water_boiling_rate;
    int sensor_reading_timer;
    int water_tank_water_max_level;
    int water_tank_water_min_level;
    int boiling_tank_water_max_level;
    int boiling_tank_water_min_level;
} system_settings_t;

typedef struct data_packet_struct{
    int message_type;
    system_settings_t system_settings;
} data_packet_t;