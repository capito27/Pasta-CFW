#pragma once

typedef int menu_func_t(int, void *);

typedef struct menu_elem_t {
	const char *name;
	menu_func_t *func;
} _menu_elem_t; 

typedef struct menu_t {
	int element_count;
	struct menu_elem_t element[];
} _menu_t;

int menu_get_element_count (struct menu_t *menu);
int menu_is_valid_index (int idx, struct menu_t *menu);
int menu_update_index (int idx, struct menu_t *menu);
const char *menu_get_element_name (int idx, struct menu_t *menu);
menu_func_t *menu_get_element_function (int idx, struct menu_t *menu);
int menu_execute_function (int idx, struct menu_t *menu, void *param);
