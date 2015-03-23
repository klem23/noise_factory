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

#ifndef NOTE_HPP
#define NOTE_HPP

#include <iostream>
#include <stdint.h>

using namespace std;

class note{

public:
	uint8_t key;
	uint32_t offset;	
	uint32_t start_offset;
	uint32_t stop_offset;	
	uint32_t release_offset;
	bool toDelete;

	note(void):key(64), offset(0), start_offset(0),stop_offset(0), release_offset(0), toDelete(false){}
	note(uint8_t key, uint32_t so):key(key), offset(0), start_offset(so),stop_offset(0), release_offset(0), toDelete(false) {}
	~note(void){}


	/*to match back with note off event*/
	static bool note_equal(note nt1, note nt2){
		if((nt1.key == nt2.key)&&(nt1.stop_offset == 0)){
			return true;
		}else{
			return false;
		}
	}

	bool operator == (note* nt){
		if((this->key == nt->key)&&(this->stop_offset == 0)){
			return true;
		}else{
			return false;
		}
	}

};
#endif

