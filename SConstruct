#!/usr/local/bin/python

############################
#Environment & Common Source
############################
#env = Environment();
env = Environment(CCFLAGS = "-g -Wall");
env['CPPPATH'] = ['src/beat_factory/', 'src/common/', 'src/generator/',  'src/filter/', 'src/mod/', 'src/fx/', 'src/gui/common/', 'src/gui/LV2/', 'src/gui/OSC/']

VariantDir('build_scons/bf/', 'src/beat_factory/', duplicate=0)
VariantDir('build_scons/common/', 'src/common/', duplicate=0)
VariantDir('build_scons/generator/', 'src/generator/', duplicate=0)
VariantDir('build_scons/mod/', 'src/mod/', duplicate=0)
VariantDir('build_scons/filter/', 'src/filter/', duplicate=0)
VariantDir('build_scons/fx/', 'src/fx/', duplicate=0)
VariantDir('build_scons/gui/common/', 'src/gui/common/', duplicate=0)
VariantDir('build_scons/gui/LV2/', 'src/gui/LV2/', duplicate=0)
VariantDir('build_scons/gui/OSC/', 'src/gui/OSC/', duplicate=0)
#VariantDir('build_scons/gui_new/', 'src/gui_new/', duplicate=0)
VariantDir('build_scons/lv2_plugin/', 'src/lv2_plugin/', duplicate=0)

#############
#Beat Factory
#############
env_bf = env.Clone();
env_bf['CCFLAGS'] = "-g -Wall -std=c99"
bf_src = Split("build_scons/bf/osc.c build_scons/bf/beat_factory.c")
env_bf.SharedLibrary('build_scons/out/beat_factory', bf_src)

##################
#Noise Factory
##################

###########
#GENERATOR
##########

#common class
common_src = env.Split("build_scons/common/midi_parser.cpp build_scons/common/tool.cpp");
common_obj = env.SharedObject(common_src);

graph_src = env.Split("build_scons/common/graph_tool.cpp");
graph_obj = env.SharedObject(graph_src);

fft_src = env.Split("build_scons/common/spectrum_fft.cpp");
fft_obj = env.SharedObject(fft_src);

#Classic OSC
classic_osc_src = Split("build_scons/generator/osc.cpp build_scons/mod/env.cpp build_scons/mod/lfo.cpp build_scons/mod/perc.cpp build_scons/lv2_plugin/lv2_classic_osc.cpp")
osc_obj = env.SharedObject(classic_osc_src);
env.SharedLibrary('build_scons/out/classic_osc', common_obj + osc_obj)

#Wave Draw
env_wd = env.Clone();
env_wd['CCFLAGS'] = "-g -Wall -DOSC"
wave_draw_src = Split("build_scons/generator/wave_gen.cpp")
wave_draw_obj = env_wd.SharedObject(wave_draw_src);
wave_draw_plugin_src = Split("build_scons/lv2_plugin/lv2_wave_draw.cpp")
wave_draw_plugin_obj = env_wd.SharedObject(wave_draw_plugin_src);
env_wd.Append(LIBS=['samplerate'])
env_wd.Append(LIBS=['lo'])
env_wd.SharedLibrary('build_scons/out/wave_draw', common_obj + graph_obj + wave_draw_obj + wave_draw_plugin_obj)

#Spectrum Draw
env_sd = env.Clone();
env_sd['CCFLAGS'] = "-g -Wall -DOSC"
spectre_draw_src = Split("build_scons/generator/spectrum_gen.cpp")
spectre_draw_obj = env_sd.SharedObject(spectre_draw_src);
spectre_draw_plugin_src = Split("build_scons/lv2_plugin/lv2_spectrum_draw.cpp")
spectre_draw_plugin_obj = env_sd.SharedObject(spectre_draw_plugin_src);
env_sd.Append(LIBS=['samplerate'])
env_sd.Append(LIBS=['fftw3'])
env_sd.Append(LIBS=['lo'])
env_sd.SharedLibrary('build_scons/out/spectrum_draw', common_obj + graph_obj + fft_obj + spectre_draw_obj + spectre_draw_plugin_obj)

#########
#FILTER
########

#recursive filter
env_rf = env.Clone();
recursive_filter_src = Split("build_scons/filter/zp_diag_filter.cpp build_scons/lv2_plugin/lv2_zp_diag_filter.cpp")
recursive_filter_obj = env_rf.SharedObject(recursive_filter_src);
env_rf.SharedLibrary('build_scons/out/recursive_filter', recursive_filter_obj)

#convolution filter
env_cf = env.Clone();
env_cf['CCFLAGS'] = "-g -Wall -DOSC"
conv_filter_src = Split("build_scons/filter/conv_filter.cpp")
conv_filter_obj = env_cf.SharedObject(conv_filter_src);
conv_filter_plugin_src = Split("build_scons/lv2_plugin/lv2_conv_filter.cpp")
conv_filter_plugin_obj = env_cf.SharedObject(conv_filter_plugin_src);
env_cf.Append(LIBS=['samplerate'])
env_cf.Append(LIBS=['fftw3'])
env_cf.Append(LIBS=['lo'])
env_cf.SharedLibrary('build_scons/out/conv_filter', graph_obj + conv_filter_obj + conv_filter_plugin_obj)

#spectrum filter
env_sf = env.Clone();
env_sf['CCFLAGS'] = "-g -Wall -DOSC"
spectre_filter_src = Split("build_scons/filter/spectrum_filter.cpp")
spectre_filter_obj = env_sf.SharedObject(spectre_filter_src);
spectre_filter_plugin_src = Split("build_scons/lv2_plugin/lv2_spectrum_filter.cpp")
spectre_filter_plugin_obj = env_sf.SharedObject(spectre_filter_plugin_src);
env_sf.Append(LIBS=['samplerate'])
env_sf.Append(LIBS=['fftw3'])
env_sf.Append(LIBS=['lo'])
env_sf.SharedLibrary('build_scons/out/spectrum_filter', graph_obj + fft_obj + spectre_filter_obj + spectre_filter_plugin_obj)


########
#SYNTH
#######

#classic synth

#wave factory
env_wf = env.Clone();
env_wf['CCFLAGS'] = "-g -Wall -DOSC"
wave_factory_src = Split("build_scons/lv2_plugin/lv2_wave_factory.cpp")
wave_factory_obj = env_wf.SharedObject(wave_factory_src);
env_wf.Append(LIBS=['samplerate'])
env_wf.Append(LIBS=['fftw3'])
env_wf.Append(LIBS=['lo'])
env_wf.SharedLibrary('build_scons/out/wave_factory', graph_obj + wave_draw_obj + conv_filter_obj + wave_factory_obj)

#spectrum factory
env_spf = env.Clone();
env_spf['CCFLAGS'] = "-g -Wall -DOSC"
spectrum_factory_src = Split("build_scons/lv2_plugin/lv2_spectrum_factory.cpp")
spectrum_factory_obj = env_spf.SharedObject(spectrum_factory_src)
env_spf.Append(LIBS=['samplerate'])
env_spf.Append(LIBS=['fftw3'])
env_spf.Append(LIBS=['lo'])
env_spf.SharedLibrary('build_scons/out/spectrum_factory', graph_obj + spectre_draw_obj + spectre_filter_obj + spectrum_factory_obj)


#########
#FX
#########

#simple delay
simple_delay_src = Split("build_scons/fx/simple_delay.cpp build_scons/lv2_plugin/lv2_simple_delay.cpp")
simple_delay_obj = env.SharedObject(simple_delay_src);
env.SharedLibrary('build_scons/out/simple_delay', simple_delay_obj)

#loop delay
loop_delay_src = Split("build_scons/fx/loop_delay.cpp build_scons/lv2_plugin/lv2_loop_delay.cpp")
loop_delay_obj = env.SharedObject(loop_delay_src);
env.SharedLibrary('build_scons/out/loop_delay', loop_delay_obj)

#pitch shifter
env_ps = env.Clone();
pitch_shifter_src = Split("build_scons/fx/pitch_shifter.cpp build_scons/lv2_plugin/lv2_pitch_shifter.cpp")
pitch_shifter_obj = env.SharedObject(pitch_shifter_src);
env_ps.Append(LIBS=['fftw3'])
env_ps.SharedLibrary('build_scons/out/pitch_shifter', pitch_shifter_obj + fft_obj)

#octaver
env_oct = env.Clone();
octaver_src = Split("build_scons/fx/octaver.cpp build_scons/lv2_plugin/lv2_octaver.cpp")
octaver_obj = env.SharedObject(octaver_src);
env_oct.Append(LIBS=['fftw3'])
env_oct.SharedLibrary('build_scons/out/octaver', octaver_obj + fft_obj)


########
#GUI
########

env_graph = env.Clone();
#env_graph['CCFLAGS'] = "-g -Wall -DLV2_GUI -DOSC"
env_graph['CCFLAGS'] = "-g -Wall"

#######
#Qt ENV
env_graph.Tool('qt');
#env_heg['QTDIR'] = '/usr/lib/qt/'
env_graph['QT_CPPPATH'] = ['/usr/include/qt4/',
                        '/usr/include/qt4/Qt/',
                        '/usr/include/qt4/QtCore/',
                        '/usr/include/qt4/QtGui/']
env_graph['QT_MOC'] = '/usr/bin/moc-qt4'
env_graph.Append(LIBS=['QtCore'])
env_graph.Append(LIBS=['QtGui'])
#Qt ENV end
#########

common_gui_src = Split("build_scons/gui/common/CurvePicker.cpp build_scons/gui/common/CurveDraw.cpp")
common_gui_obj = env_graph.SharedObject(common_gui_src);
env_graph.Append(LIBS=['qwt'])
env_graph.Append(LIBS=['lo'])

#Wave Draw GUI
wave_draw_gui_lv2_src = Split("build_scons/gui/LV2/LV2UI.cpp  build_scons/gui/LV2/lv2_wave_draw_gui.cpp")
wave_draw_gui_lv2_obj = env_graph.SharedObject(wave_draw_gui_lv2_src);
env_graph.SharedLibrary('build_scons/out/wave_draw_gui', common_gui_obj + wave_draw_gui_lv2_obj)

wave_draw_gui_sa_src = Split("build_scons/gui/OSC/OSCUI.cpp build_scons/gui/OSC/wave_gmain.cpp")
wave_draw_gui_sa_obj = env_graph.SharedObject(wave_draw_gui_sa_src);
env_graph.Program('build_scons/gui/wave_draw_gui_sa', common_gui_obj + wave_draw_gui_sa_obj)

#Spectrum Draw GUI
spectrum_draw_gui_lv2_src = Split("build_scons/gui/LV2/LV2UI.cpp  build_scons/gui/LV2/lv2_spectrum_draw_gui.cpp")
spectrum_draw_gui_lv2_obj = env_graph.SharedObject(spectrum_draw_gui_lv2_src);
env_graph.SharedLibrary('build_scons/out/spectrum_draw_gui', common_gui_obj + spectrum_draw_gui_lv2_obj)
spectrum_draw_gui_sa_src = Split("build_scons/gui/OSC/OSCUI.cpp build_scons/gui/OSC/spectrum_gmain.cpp")
spectrum_draw_gui_sa_obj = env_graph.SharedObject(spectrum_draw_gui_sa_src);
env_graph.Program('build_scons/gui/spectrum_draw_gui_sa', common_gui_obj + spectrum_draw_gui_sa_obj)

#convolution filter gui
cfilter_gui_lv2_src = Split("build_scons/gui/OSC/OSCUI.cpp build_scons/gui/LV2/lv2_cfilter_gui.cpp")
cfilter_gui_lv2_obj = env_graph.SharedObject(cfilter_gui_lv2_src);
env_graph.SharedLibrary('build_scons/out/conv_filter_gui', common_gui_obj + cfilter_gui_lv2_obj)
cfilter_gui_sa_src = Split("build_scons/gui/OSC/OSCUI.cpp build_scons/gui/OSC/cfilter_gmain.cpp")
cfilter_gui_sa_obj = env_graph.SharedObject(cfilter_gui_sa_src);
env_graph.Program('build_scons/gui/conv_filter_gui_sa', common_gui_obj + cfilter_gui_sa_obj)

#spectrum filter gui
sfilter_gui_lv2_src = Split("build_scons/gui/OSC/OSCUI.cpp build_scons/gui/LV2/lv2_sfilter_gui.cpp")
sfilter_gui_lv2_obj = env_graph.SharedObject(sfilter_gui_lv2_src);
env_graph.SharedLibrary('build_scons/out/spectrum_filter_gui', common_gui_obj + sfilter_gui_lv2_obj)
sfilter_gui_sa_src = Split("build_scons/gui/OSC/OSCUI.cpp build_scons/gui/OSC/sfilter_gmain.cpp")
sfilter_gui_sa_obj = env_graph.SharedObject(sfilter_gui_sa_src);
env_graph.Program('build_scons/gui/spectrum_filter_gui_sa', common_gui_obj + sfilter_gui_sa_obj)

##########
#Bundle
##########

#copy ttl
Command("build_scons/noise_factory.lv2/manifest.ttl","ttl/manifest.ttl",
                         Copy("$TARGET","$SOURCE"))

Command("build_scons/noise_factory.lv2/beat_factory.ttl","ttl/beat_factory.ttl",
                         Copy("$TARGET","$SOURCE"))

Command("build_scons/noise_factory.lv2/classic_osc.ttl","ttl/classic_osc.ttl",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/wave_draw.ttl","ttl/wave_draw.ttl",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/spectrum_draw.ttl","ttl/spectrum_draw.ttl",
                         Copy("$TARGET","$SOURCE"))

Command("build_scons/noise_factory.lv2/recursive_filter.ttl","ttl/recursive_filter.ttl",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/conv_filter.ttl","ttl/conv_filter.ttl",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/spectrum_filter.ttl","ttl/spectrum_filter.ttl",
                         Copy("$TARGET","$SOURCE"))

Command("build_scons/noise_factory.lv2/simple_delay.ttl","ttl/simple_delay.ttl",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/loop_delay.ttl","ttl/loop_delay.ttl",
                         Copy("$TARGET","$SOURCE"))

Command("build_scons/noise_factory.lv2/pitch_shifter.ttl","ttl/pitch_shifter.ttl",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/octaver.ttl","ttl/octaver.ttl",
                         Copy("$TARGET","$SOURCE"))


#copy lib
Command("build_scons/noise_factory.lv2/beat_factory.so","build_scons/out/libbeat_factory.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/classic_osc.so","build_scons/out/libclassic_osc.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/wave_draw.so","build_scons/out/libwave_draw.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/spectrum_draw.so","build_scons/out/libspectrum_draw.so",
                         Copy("$TARGET","$SOURCE"))


Command("build_scons/noise_factory.lv2/recursive_filter.so","build_scons/out/librecursive_filter.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/conv_filter.so","build_scons/out/libconv_filter.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/spectrum_filter.so","build_scons/out/libspectrum_filter.so",
                         Copy("$TARGET","$SOURCE"))


Command("build_scons/noise_factory.lv2/simple_delay.so","build_scons/out/libsimple_delay.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/loop_delay.so","build_scons/out/libloop_delay.so",
                         Copy("$TARGET","$SOURCE"))

Command("build_scons/noise_factory.lv2/pitch_shifter.so","build_scons/out/libpitch_shifter.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/octaver.so","build_scons/out/liboctaver.so",
                         Copy("$TARGET","$SOURCE"))

#copy gui
Command("build_scons/noise_factory.lv2/wave_draw_gui.so","build_scons/out/libwave_draw_gui.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/spectrum_draw_gui.so","build_scons/out/libspectrum_draw_gui.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/conv_filter_gui.so","build_scons/out/libconv_filter_gui.so",
                         Copy("$TARGET","$SOURCE"))
Command("build_scons/noise_factory.lv2/spectrum_filter_gui.so","build_scons/out/libspectrum_filter_gui.so",
                         Copy("$TARGET","$SOURCE"))
