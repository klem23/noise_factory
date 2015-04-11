/*
	This file is part of the Noise Factory
	Copyright (C) 2006-2015 Clément Guedez <klem.dev@gmail.com>


    The Noise Factory is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Noise Factory is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Noise Factory.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LV2_UI_HPP
#define LV2_UI_HPP

#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

class LV2UI{

private:
	LV2UI_Controller lv2_ctrl;
	LV2UI_Write_Function lv2_write;
	float* data;

protected:
	LV2UI();

	void set_lv2_ctrl(LV2UI_Controller lc);
	void set_lv2_write_fn(LV2UI_Write_Function lw);

	void lv2_send_curve(float* curve, uint32_t size);
	void lv2_send_data(uint32_t index, double data);

};


#endif // LV2_UI_HPP
