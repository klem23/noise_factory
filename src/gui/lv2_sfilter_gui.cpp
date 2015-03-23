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

#include <cstdlib>
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"

#include "wave_draw.hpp"

#define SFUI_URI "http://lv2plug.in/plugins/noise_factory/spectrum_filter/gui";

using namespace std;

LV2UI_Descriptor *sfUIDesc = NULL;

LV2UI_Handle gui_instantiate(const struct _LV2UI_Descriptor* descriptor,
                          const char*                     plugin_uri,
                          const char*                     bundle_path,
                          LV2UI_Write_Function            write_function,
                          LV2UI_Controller                controller,
                          LV2UI_Widget*                   widget,
                          const LV2_Feature* const*       features)
{
    WaveDraw* wd = new WaveDraw(NULL, 4000, FILTER);
    wd->show();
    *widget = wd;

/*
        LV2_URID_Map* map;
        LV2_URID  atom_vector_urid;

	for (int i = 0; features[i]; ++i) {
		if (!strcmp(features[i]->URI, LV2_URID_URI "#map")) {
                        map = (LV2_URID_Map*)features[i]->data;
                        //atom_vector_urid = map->map(map->handle, LV2_ATOM__Vector);
                        atom_vector_urid = map->map(map->handle, LV2_ATOM__eventTransfer);
                        wb->set_lv2_urid(atom_vector_urid);
                        printf("match\n");
                }
        }
*/

    return (LV2UI_Handle)wd;
}

void gui_cleanup(LV2UI_Handle handle)
{
	WaveDraw* wd = (WaveDraw*)handle;
	delete wd;

}

void gui_port_event(LV2UI_Handle handle, uint32_t port, uint32_t buffer_size, uint32_t format, const void *buffer)
{

}

const void *gui_extension(const char *uri)
{
    return NULL;
}


void init(void){
    sfUIDesc = (LV2UI_Descriptor*)malloc(sizeof(LV2UI_Descriptor));

    sfUIDesc->URI = SFUI_URI;
    sfUIDesc->instantiate = gui_instantiate;
    sfUIDesc->cleanup = gui_cleanup;
    sfUIDesc->port_event = gui_port_event;
    sfUIDesc->extension_data = gui_extension;
}


const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index)
{

	if(!sfUIDesc) init();

        switch(index){
        case 0:
                return sfUIDesc;
        default:
                return NULL;
        }

}


