#include <stdio.h>
#include "conio_linux.h"

void print_boolean_sensor_data(uint8_t boolean_data){
    if(boolean_data){
            setfontcolor(GREEN);
        } else {
            setfontcolor(RED);
        }
    printf("%s", boolean_data? "ON " : "OFF");
    setfontcolor(WHITE);
}

void print_number_sensor_data(uint8_t sensor_data){
    if(sensor_data){
        setfontcolor(GREEN);
    } else {
        setfontcolor(RED);
    }
    printf("%d ", sensor_data);
    setfontcolor(WHITE);
}

void print_tank_temperature(uint8_t tank_temp){
    if(40 >= tank_temp){
        setfontcolor(BLUE);
    } else {
        if(40 < tank_temp && 65 >= tank_temp){
            setfontcolor(YELLOW);
        } else {
            if(65 < tank_temp){
                setfontcolor(RED);
            }
        }
    }
    printf("%d°C\n", tank_temp);
}

void set_water_color(uint8_t water_temp){
    if(40 >= water_temp){
        setfontcolor(BLUE);
    } else {
        if(40 < water_temp && 65 >= water_temp){
            setfontcolor(YELLOW);
        } else {
            if(65 < water_temp){
                setfontcolor(RED);
            }
        }
    }
};

void print_common_water_level(uint8_t minimun_water_level, uint8_t water_level, uint8_t water_temp){
    printf("║");
    if(minimun_water_level <= water_level){
        //║░░░░░░░░║
        set_water_color(water_temp);
        printf("░░░░░░░░");
        setfontcolor(WHITE);
    } else {
        //║        ║;
        setfontcolor(WHITE);
        printf("        ");
    }
    printf("║");
}

void valve_print(uint8_t x, uint8_t y, uint8_t valve_status,
                 uint8_t previous_tank_water_level, uint8_t previous_tank_water_temp,
                 uint8_t next_tank_water_level, uint8_t next_tank_water_temp){
    gotoxy(x+5,y+6);
    if(valve_status){
        setfontcolor(GREEN);
    } else {
        setfontcolor(RED);
    }
    printf("%s", valve_status? "ON " : "OFF");
    gotoxy(x,y+7);
    setfontcolor(WHITE);
    gotoxy(x,y+8);
    printf("═════╦═════");
    gotoxy(x,y+9);
    if(valve_status){
        set_water_color(previous_tank_water_temp);
        printf("░░░░░");
        set_water_color(next_tank_water_temp);
        printf("░░░░░░");
    } else {
        if(5 > previous_tank_water_level){
            printf("     ");
        } else {
            set_water_color(previous_tank_water_temp);
            printf("░░░░░");
        }
        setfontcolor(WHITE);
        printf("║");
        if(5 >= next_tank_water_level){
            printf("     ");
        } else {
            set_water_color(next_tank_water_temp);
            printf("░░░░░");
        }
    }
    setfontcolor(WHITE);
    gotoxy(x,y+10);
    printf("═════╩═════");
}

void tank_print(uint8_t x, uint8_t y, uint8_t water_level,
                uint8_t water_temp, uint8_t resistance_status,
                uint8_t has_resistance, uint8_t max_sensor_status,
                uint8_t min_sensor_status){
    gotoxy(x+15,y);
    printf("%d",water_level);
    printf("%%");
    gotoxy(x+11,y+1);

    gotoxy(x+11,y+2);
    printf("╔════════╗");
    gotoxy(x+11,y+3);
    printf("║");
    if(max_sensor_status){
    setfontcolor(GREEN);
    } else {
        setfontcolor(RED);
    }
    printf("■");
    if(95 <= water_level){
        //║■░░░░░█░║
        set_water_color(water_temp);
        printf("░░░░░");
        if(has_resistance){
            if(resistance_status){
                setfontcolor(RED);
            } else {
                setfontcolor(WHITE);
            }
            printf("█");
            set_water_color(water_temp);
            printf("░");
        } else {
            printf("░░");
        }
        setfontcolor(WHITE);
    } else {
        //║■     █ ║
        setfontcolor(WHITE);
        printf("     ");
        if(has_resistance){
            if(resistance_status){
                setfontcolor(RED);
            } else {
                setfontcolor(WHITE);
            }
            printf("█");
            set_water_color(water_temp);
            printf(" ");
        } else {
            printf("  ");
        }
    }
    setfontcolor(WHITE);
    printf("║");
    gotoxy(x+11,y+4);

    print_common_water_level(90,water_level,water_temp);
    gotoxy(x+11,y+5);
    print_common_water_level(80,water_level,water_temp);
    gotoxy(x+11,y+6);
    print_common_water_level(60,water_level,water_temp);
    gotoxy(x+11,y+7);
    print_common_water_level(40,water_level,water_temp);
    gotoxy(x+11,y+8);
    //╬■░░░░░░░╬
    printf("╬");
    if(min_sensor_status){
    setfontcolor(GREEN);
    } else {
        setfontcolor(RED);
    }
    printf("■");
    if(20 <= water_level){
        set_water_color(water_temp);
        printf("░░░░░░░");
    } else {
        //╬■       ╬
        printf("       ");
    }
    setfontcolor(WHITE);
    printf("╬");
    gotoxy(x+11,y+9);
    //░░░░░░░░░░
    if(0 < water_level){
        set_water_color(water_temp);
        printf("░░░░░░░░░░");
    } else {
        printf("          ");
    }
    setfontcolor(WHITE);
    gotoxy(x+11,y+10);
    printf("══════════");
}