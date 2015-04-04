#!/usr/bin/python
top = '.'
out = 'build_waf'


def options(opt):
        opt.load('compiler_c compiler_cxx qt4')
def configure(cnf):
        cnf.load('compiler_c compiler_cxx qt4')
        cnf.check(features='c', lib=['m'], cflags=['-g', '-Wall', '-std=c99'], uselib_store='cbase')
        cnf.check(features='cxx', lib=['m'], cxxflags=['-g', '-Wall'], uselib_store='cxxbase')
        cnf.check(features='cxx', lib=['m','samplerate'], cxxflags=['-g', '-Wall'], uselib_store='wave')
        cnf.check(features='cxx', lib=['m','fftw3'], cxxflags=['-g', '-Wall',], uselib_store='spectre')
        cnf.check(features='cxx', lib=['lo'], cxxflags=['-g', '-Wall', '-D OSC'], uselib_store='OSC')
        cnf.check(features='cxx', lib=['gui','qwt'], cxxflags=['-g', '-Wall', '-D LV2_GUI'], uselib_store='lv2_gui')
        #cnf.check(features='cxx qt4', lib=['qwt'], cxxflags=['-g', '-Wall'], ldflags=['-lqwt'], uselib_store='gui_qt')


	cnf.check_cfg (package='samplerate', uselib_store='wave_2', args='--cflags --libs')
	cnf.check_cfg (package='fftw3', uselib_store='spectre_2', args='--cflags --libs')
	#cnf.check_cfg (package='Qt', uselib_store='gui', args='--cflags --libs')
	cnf.check_cfg (package='liblo', uselib_store='lo', args='--cflags --libs')


def build(bld):

	bld(features='c cshlib',
		source       = 'src/beat_factory/osc.c src/beat_factory/beat_factory.c',
		target       = 'dsp/beat_factory',
		use          = ['cbase']
	)

        bld(features='cxx cxxshlib',
                source       = 'src/common/midi_parser.cpp src/generator/osc.cpp src/mod/perc.cpp',
		includes     = 'src/common/ src/mod/ src/generator/',
                target       = 'dsp/beat_factory_2',
                use          = ['cxxbase']
        )

	bld(features='cxx cxxshlib',
                source       = 'src/common/midi_parser.cpp src/generator/osc.cpp src/lv2_plugin/lv2_classic_osc.cpp src/mod/env.cpp src/mod/lfo.cpp src/mod/perc.cpp src/common/tool.cpp', 
		includes     = 'src/common/ src/mod/ src/generator/',
                target       = 'dsp/classic_osc', 
		use	     = ['cxxbase']
        )

        bld(features='cxx cxxshlib',
                source       = 'src/common/midi_parser.cpp src/common/tool.cpp src/common/graph_tool.cpp src/generator/wave_gen.cpp src/lv2_plugin/lv2_wave_draw.cpp', 
		includes     = 'src/common/ src/generator/',
                target       = 'dsp/wave_draw', 
                use          = ['wave', 'wave_2', 'OSC', 'lo']
        )

        bld(features='cxx cxxshlib',
                source       = 'src/common/midi_parser.cpp src/generator/spectrum_gen.cpp src/common/graph_tool.cpp src/lv2_plugin/lv2_spectrum_draw.cpp src/common/spectrum_fft.cpp',
		includes     = 'src/common/ src/generator/',
                target       = 'dsp/spectrum_draw',
                use          = ['spectre', 'spectre_2', 'OSC', 'lo', 'wave_2']
        )

#FILTER
        bld(features='cxx cxxshlib',
                source       = 'src/filter/zp_diag_filter.cpp src/lv2_plugin/lv2_zp_diag_filter.cpp',
		includes     = 'src/filter/',
                target       = 'dsp/recursive_filter',
                use          = ['cxxbase']
        )

        bld(features='cxx cxxshlib',
                source       = 'src/filter/spectrum_filter.cpp src/common/spectrum_fft.cpp src/common/graph_tool.cpp src/lv2_plugin/lv2_spectrum_filter.cpp',
                includes     = 'src/filter/ src/common/',
                target       = 'dsp/spectrum_filter',
                use          = ['spectre', 'spectre_2', 'wave_2', 'OSC', 'lo']
        )

        bld(features='cxx cxxshlib',
                source       = 'src/filter/conv_filter.cpp src/common/graph_tool.cpp src/lv2_plugin/lv2_conv_filter.cpp',
                includes     = 'src/filter/ src/common/',
                target       = 'dsp/conv_filter',
                use          = ['spectre', 'spectre_2', 'wave_2', 'OSC', 'lo']
        )

#SYNTH
        bld(features='cxx cxxshlib',
                source       = 'src/common/midi_parser.cpp src/generator/osc.cpp src/mod/env.cpp src/mod/lfo.cpp src/mod/perc.cpp src/filter/zp_diag_filter.cpp src/lv2_plugin/classic_synth.cpp', 
		includes     = 'src/common/ src/generator/ src/mod/ src/filter/',
		target	     = 'dsp/classic_synth',
                use          = ['cxxbase']
	)

#GUI
	bld(features='cxx cxxshlib qt4',
                source       = 'src/gui/wave_picker.cpp src/gui/wave_draw.cpp src/gui/lv2_wave_draw_gui.cpp',
                target       = 'gui/wave_draw_gui',
		includes     = 'src/gui/ /usr/local/include qt4/Qt/ qt4/QtGui/',
                uselib       = "QTCORE QTGUI",
                lib          = ['qwt'],
                use          = ['gui', 'cxxbase', 'lv2_gui']
        )

        bld(features='cxx cxxprogram qt4',
                source       = 'src/gui/wave_picker.cpp src/gui/wave_draw.cpp src/gui/wave_gmain.cpp',
                target       = 'gui_bin/wave_gen_gui',
                includes     = "qt4/Qt/ qt4/QtGui/",
                uselib       = "QTCORE QTGUI",
                lib          = ['qwt'],
                use          = ['gui', 'cxxbase', 'OSC', 'lo']
        )

        bld(features='cxx cxxprogram qt4',
                source       = 'src/gui/wave_picker.cpp src/gui/wave_draw.cpp src/gui/spectrum_gmain.cpp',
                target       = 'gui_bin/spectrum_gen_gui',
                includes     = "qt4/Qt/ qt4/QtGui/",
                uselib       = "QTCORE QTGUI",
                lib          = ['qwt'],
                use          = ['gui', 'cxxbase', 'OSC', 'lo']
        )

        bld(features='cxx cxxprogram qt4',
                source       = 'src/gui/wave_picker.cpp src/gui/wave_draw.cpp src/gui/sfilter_gmain.cpp',
                target       = 'gui_bin/sfilter_gui',
                includes     = "qt4/Qt/ qt4/QtGui/",
                uselib       = "QTCORE QTGUI",
                lib          = ['qwt'],
                use          = ['gui', 'cxxbase', 'OSC', 'lo']
        )

        bld(features='cxx cxxprogram qt4',
                source       = 'src/gui/wave_picker.cpp src/gui/wave_draw.cpp src/gui/cfilter_gmain.cpp',
                target       = 'gui_bin/cfilter_gui',
                includes     = "qt4/Qt/ qt4/QtGui/",
                uselib       = "QTCORE QTGUI",
                lib          = ['qwt'],
                use          = ['gui', 'cxxbase', 'OSC', 'lo']
        )

        #bld(features='cxx cxxprogram qt4',
        #        source       = 'src/gui_new/wave_draw.cpp src/gui_new/wave_picker.cpp src/gui_new/main.cpp',
		#includes     = bld.env.INCLUDES_QTGUI,
	#	includes     = "qt4/Qt/ qt4/QtGui/ src/gui_new/",
	#	uselib	     = "QTCORE QTGUI QWT",
	#	lib	     = ['qwt'],
         #       target       = 'gui/qt_gui',
         #       use          = ['cxxbase', 'gui_qt']
        #)

#FX
        bld(features='cxx cxxshlib',
                source       = 'src/fx/simple_delay.cpp src/lv2_plugin/lv2_simple_delay.cpp',
		includes     = 'src/fx/',
                target       = 'dsp/simple_delay',
                use          = ['cxxbase']
        )

        bld(features='cxx cxxshlib',
                source       = 'src/fx/loop_delay.cpp src/lv2_plugin/lv2_loop_delay.cpp',
		includes     = 'src/fx/',
                target       = 'dsp/loop_delay',
                use          = ['cxxbase']
        )


#Copy engine plugin
	bld(rule='cp ${SRC} ${TGT}', source='dsp/libbeat_factory.so', target='noise_factory.lv2/beat_factory.so')

	bld(rule='cp ${SRC} ${TGT}', source='dsp/libclassic_osc.so', target='noise_factory.lv2/classic_osc.so')

	bld(rule='cp ${SRC} ${TGT}', source='dsp/libwave_draw.so', target='noise_factory.lv2/wave_draw.so')

	bld(rule='cp ${SRC} ${TGT}', source='dsp/libspectrum_draw.so', target='noise_factory.lv2/spectrum_draw.so')

#Copy filter plugin
	bld(rule='cp ${SRC} ${TGT}', source='dsp/librecursive_filter.so', target='noise_factory.lv2/recursive_filter.so')

	bld(rule='cp ${SRC} ${TGT}', source='dsp/libspectrum_filter.so', target='noise_factory.lv2/spectrum_filter.so')
	bld(rule='cp ${SRC} ${TGT}', source='dsp/libconv_filter.so', target='noise_factory.lv2/conv_filter.so')

#Copy synth plugin
	bld(rule='cp ${SRC} ${TGT}', source='dsp/libclassic_synth.so', target='noise_factory.lv2/classic_synth.so')

#Copy gui plugin
	bld(rule='cp ${SRC} ${TGT}', source='gui/libwave_draw_gui.so', target='noise_factory.lv2/wave_draw_gui.so')

#Copy fx plugin

	bld(rule='cp ${SRC} ${TGT}', source='dsp/libsimple_delay.so', target='noise_factory.lv2/simple_delay.so')

	bld(rule='cp ${SRC} ${TGT}', source='dsp/libloop_delay.so', target='noise_factory.lv2/loop_delay.so')

#Coopy ttl file
	#engine
	bld(rule='cp ${SRC} ${TGT}', source='ttl/manifest.ttl', target='noise_factory.lv2/')

	bld(rule='cp ${SRC} ${TGT}', source='ttl/beat_factory.ttl', target='noise_factory.lv2/')

	bld(rule='cp ${SRC} ${TGT}', source='ttl/classic_osc.ttl', target='noise_factory.lv2/')

	bld(rule='cp ${SRC} ${TGT}', source='ttl/wave_draw.ttl', target='noise_factory.lv2/')

	bld(rule='cp ${SRC} ${TGT}', source='ttl/spectrum_draw.ttl', target='noise_factory.lv2/')

	#filter
	bld(rule='cp ${SRC} ${TGT}', source='ttl/recursive_filter.ttl', target='noise_factory.lv2/')

	bld(rule='cp ${SRC} ${TGT}', source='ttl/conv_filter.ttl', target='noise_factory.lv2/')

	bld(rule='cp ${SRC} ${TGT}', source='ttl/spectrum_filter.ttl', target='noise_factory.lv2/')

	#synth
	bld(rule='cp ${SRC} ${TGT}', source='ttl/classic_synth.ttl', target='noise_factory.lv2/')

	#fx
	bld(rule='cp ${SRC} ${TGT}', source='ttl/simple_delay.ttl', target='noise_factory.lv2/')

	bld(rule='cp ${SRC} ${TGT}', source='ttl/loop_delay.ttl', target='noise_factory.lv2/')


