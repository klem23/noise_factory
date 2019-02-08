#include "siren.hpp"
//#include "note.hpp"
#include <iostream>
#include <cmath>

siren::siren(uint32_t sampling_rate)
:osc(sampling_rate)
,kmod(SWITCH)
,kpitch(false)
,pitch(500)
{

}

/*
siren::~siren(){
}*/

void siren::note_interceptor(void){
	

	if(kmod == SWITCH){
		if(note_active.size() == 1){
			note nt = note_active.front();
			if(nt.stop_offset != 0){
				note_active.clear();
				nt.stop_offset = 0;
				note_active.push_back(nt);
			}
		}else if(note_active.size() > 1){
			note_active.clear();
		}

	}else if(kmod == ALARM){
		if(note_active.size() > 1){
			note nt = note_active.back();
			note_active.clear();
			note_active.push_back(nt);
		}
	}else{
		/*you're a synthetiser Harry*/
	}
}


void siren::freq_interceptor(float* freq_mod_buff, int nb_sample){

	if((!kpitch)||(kmod == ALARM)){
		note nt = note_active.front();
		uint64_t j = nt.offset;
		uint64_t attack = ATTACK_TIME * srate;
                uint64_t k = nt.release_offset;
		uint64_t release_tot = release_time + anticlip_time;

		float antipitch = pitch / (freq_table[nt.key] * srate);

		for(uint32_t i = nt.start_offset ; i < nb_sample ; i++){
			if(kmod == ALARM){
				//attack
				if(j < attack){
					freq_mod_buff[i] = (float)j / (float)attack;
					j++;
				}
				//release
				else if(nt.stop_offset != 0){
					freq_mod_buff[i] = expf( - (float)k  /  (float)release_tot);
					k++;
				}
			}

			if(!kpitch) freq_mod_buff[i] *= antipitch;
		}


	}
}


void siren::si_check_param(si_param* spi, osc_param* op){

//	if((kmod == TONE ) || (kmod == SWITCH))
        *op->fl.active = 1.0;

	if((kmod == TONE ) || (kmod == ALARM)) {
		*op->ae.active = 1.0;
		*op->ae.amt = 1.0;
	}

	check_param(op);



	if((*spi->kmod >= 0) && (*spi->kmod < 1) && (kmod != TONE)){
                kmod = TONE;
		note_active.clear();
	}else if((*spi->kmod >= 1) && (*spi->kmod < 2) && (kmod != SWITCH)){
                kmod = SWITCH;
		note_active.clear();
	}else if((*spi->kmod >= 2) && (*spi->kmod <= 3) && (kmod != ALARM)){
                kmod = ALARM;
		note_active.clear();
	}

	if((*spi->kpitch) && (!kpitch)){
                kpitch = true;
        }else if((*spi->kpitch == 0) && (kpitch)){
                kpitch = false;
        }

        if(*spi->pitch != pitch){
                pitch = *spi->pitch;
        }

}

void siren::process(int nb_sample){

	process_midi(nb_sample);
	note_interceptor();
	process_audio(nb_sample);
}
