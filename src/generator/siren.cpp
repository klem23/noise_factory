#include "siren.hpp"
//#include "note.hpp"


siren::siren(uint32_t sampling_rate)
	:osc(sampling_rate)
	,time_up(5) ,freq_up(5)
	,time_down(2), freq_down(10)
	,vol(0.8){
}

siren::~siren(){
}

void siren::note_interceptor(void){
	//if(note_active.size == 1)
	for(it = note_active.begin(); it != note_active.end()){
		if(it->start_offset != 0){
			note* nt = new note(it->key, it->start_offset);
			note_active.clear();
			note_active.push(nt);
			break;
		}else if(it->stop_offset != 0){
			it = note_active.erase(it);
		}else{
			it++;
		}
	}
}

void siren::freq_interceptor(float* freq_mod_buff, int nb_sample){
	it = not_active.begin();
	for(int i = it->start_offset; i < nb_sample; i++){
		if( i / srate < time_up){
			freq_mod_buff[i] *= it->offset * freq_up / (freq_table[it->key] * srate);
		}else if( i / srate < time_down){
			freq_mod_buff[i] *= it->offset * freq_down / (freq_table[it->key] * srate);

		}else{
			it = note_active.begin();
			it->toDelete = true;
		}
	}
}

void siren::si_check_param(si_param* spi, osc_param* op){
	check_param(op);

}

void siren::process(int nb_sample){

	process_midi(int nb_sample);
	process_interceptor(void);
	process_audio(int nb_sample);
}
