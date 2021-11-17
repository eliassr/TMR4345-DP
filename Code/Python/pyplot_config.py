from matplotlib import cycler

params = {
        'figure.figsize': (8.0, 5.0),
        'axes.grid': True,
        'grid.linestyle': 'dotted',

        'lines.markersize': 5,
        'lines.markeredgewidth': 0.5,
        'lines.markeredgecolor': 'black',
        'lines.linewidth': 1.5,

        'axes.prop_cycle': cycler('color',[
            'royalblue',
            'darkorange',
            'chartreuse',
            'LightSlateGrey',
            'red',
            '#00F0FF',
            '#D2691E',
            '#9932CC',
            'hotpink',
            '#005c7b',
            'yellow'
        ]) + cycler('markerfacecolor',[
            'darkorange',
            'chartreuse',
            'LightSlateGrey',
            'red',
            '#00F0FF',
            '#D2691E',
            '#9932CC',
            'hotpink',
            '#005c7b',
            'yellow',
            'royalblue'
        ]),

        # Tex options, enable tex-markup for math. CM font is optional
        'text.usetex': False,
        #'font.cursive': 'Computer Modern Roman',
        #'font.family': 'cursive',

        # Fig specs, will appear with plt.show()
        'font.size': 11,
        'legend.title_fontsize': 11,
        'figure.dpi': 400,
        'figure.facecolor': 'c4c4c4',


        # Savefig specs, will appear upon saving figure
        'savefig.transparent': True,
        'savefig.facecolor': 'dadada',
        }