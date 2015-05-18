#include "textmenu.h"
#include "draw.h"

int menu_get_element_count (struct menu_t *menu) {
	int i = 0;

	if (menu) {
		i = menu->element_count;
	}
	return i;
}

int menu_is_valid_index (int idx, struct menu_t *menu) {
	return (menu != 0 && (idx >= 0 && idx < menu_get_element_count(menu)));
}

int menu_update_index (int idx, struct menu_t *menu) {
	int newidx = 0;
	int count = menu_get_element_count(menu);
	
	newidx = idx < 0 ? count-1:idx >= count ? 0:idx;
	
	return newidx;
}

const char *menu_get_element_name (int idx, struct menu_t *menu) {	
	return menu_is_valid_index(idx, menu) ? menu->element[idx].name : 0;
}

menu_func_t *menu_get_element_function (int idx, struct menu_t *menu) {
	return menu_is_valid_index(idx, menu) ? menu->element[idx].func : 0;
}

int menu_execute_function (int idx, struct menu_t *menu, void *param) {
	int result = 0;
	menu_func_t *f;	

	if (menu_is_valid_index(idx, menu)) {
		f = menu_get_element_function(idx, menu);
		if (f)
			result = f(idx, param);
	}

	return result;
}
