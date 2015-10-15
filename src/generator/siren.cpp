#include "siren.hpp"
//#include "note.hpp"
#include <iostream>

siren::siren(uint32_t sampling_rate)
	:osc(sampling_rate), pitched(true)
	,time_up(5.2) ,freq_up(1000)
	,time_down(3.3), freq_down(200){
}

/*
siren::~siren(){
}*/

void siren::note_interceptor(void){
	//if(note_active.size == 1)
	it = note_active.begin();
	//for(it = note_active.begin(); it != note_active.end();){
	while(it != note_active.end()){
		if(it->start_offset != 0){
			note* nt = new note(it->key, it->start_offset);
			note_active.clear();
			note_active.push_back(*nt);
			delete nt;
			return;
		}else if((it->stop_offset != 0) && (it->stop_offset != 1)){
			//it = note_active.erase(it);
			it->stop_offset = 0;
		}else{
			it++;
		}
	}
}

void siren::freq_interceptor(float* freq_mod_buff, int nb_sample){

	it = note_active.begin();
	float den = (float)srate * (float)srate * freq_table[it->key];


	for(uint32_t i = it->start_offset ; i < nb_sample ; i++){
		float off_idx = i + it->offset - it->start_offset;	
		if( off_idx / (float)srate < time_up){
			if(pitched){
				freq_mod_buff[i] *= off_idx * freq_up / ( den * time_up) + 1.0 ;
			} else {
				freq_mod_buff[i] *= off_idx * freq_up / ( den * time_up);
			}
		}else if( off_idx / (float)srate < time_up + time_down){
			if(pitched){
				freq_mod_buff[i] *= ((time_down + time_up) * (float)srate - off_idx) * (freq_up - freq_down) / (den * time_down) + freq_down * (float)srate / den + 1.0;
			} else {
				freq_mod_buff[i] *= ((time_down + time_up) * (float)srate - off_idx) * (freq_up - freq_down) / (den * time_down) + freq_down * (float)srate / den;
			}
		}else{
			if(pitched){
				freq_mod_buff[i] *= freq_down * (float)srate / den + 1.0;
			}else{
				freq_mod_buff[i] *= freq_down * (float)srate / den;

			}
			it->stop_offset = 1;
		}
	}

}

void siren::si_check_param(si_param* spi, osc_param* op){
	check_param(op);

	if((*spi->pitched > 0)&&(!pitched)){
                pitched = true;
        }else if((*spi->pitched <= 0)&&(pitched)){
                pitched = false;
        }


        if(*spi->time_up != time_up){
                time_up = *spi->time_up;
        }

        if(*spi->freq_up != freq_up){
                freq_up = *spi->freq_up;
        }

        if(*spi->time_down != time_down){
                time_down = *spi->time_down;
        }

        if(*spi->freq_down != freq_down){
                freq_down = *spi->freq_down;
        }




}

void siren::process(int nb_sample){

	process_midi(nb_sample);
	note_interceptor();
	process_audio(nb_sample);
}
