/******************************************************************************
*
* Tilengine sample
* 2016 Marc Palacios
* http://www.tilengine.org
*
* This example show a classic sidescroller. It mimics the actual Sega Genesis
* Sonic game. It uses two layers, where the background one has multiple strips
* and a linescroll effect. It also uses color animation for the water cycle
*
******************************************************************************/

using Tilengine;

class Platformer
{
    // constants
    const int Hres = 400;
    const int Vres = 240;
    const int BackgroundStrips = 6;

    // singleton instances
    static Engine engine;
    static Window window;

    static Color[] skyColor = new Color[]
    {
	    new Color(0x1B, 0x00, 0x8B),    // top color
	    new Color(0x00, 0x74, 0xD7),    // bottom color
    };

    static Color[] waterColor = new Color[]
    {
	    new Color(0x24, 0x92, 0xDB),    // top color
	    new Color(0x1F, 0x7F, 0xBE),    // bottom color
    };

    static float posForeground = 0;
    static float[] posBackground = new float[BackgroundStrips];
    static float[] incBackground = new float[BackgroundStrips] { 0.562f, 0.437f, 0.375f, 0.625f, 1.0f, 2.0f };
    static Layer foreground;
    static Layer background;

    /* helper for loading a related tileset + tilemap and configure the appropiate layer */
    static void LoadLayer (Layer layer, string filename)
    {
        Tilemap tilemap = Tilemap.FromFile(filename, null);
        layer.SetMap (tilemap);
    }

    /* entry point */
    static int Main(string[] args)
    {
	    int c;
        int frame = 0;
        float speed = 0;

	    /* setup engine */
        engine = Engine.Init(Hres, Vres, 2, 0, 1);
        foreground = engine.Layers[0];
        background = engine.Layers[1];

	    /* load resources*/
        LoadLayer(foreground, "Sonic_md_fg1.tmx");
        LoadLayer(background, "Sonic_md_bg1.tmx");

        // load sequences for animations
	    SequencePack sp = SequencePack.FromFile("Sonic_md_seq.sqx");
	    Sequence waterSequence = sp.Find ("seq_water");
        Palette palette = background.Palette;
        engine.Animations[0].SetPaletteAnimation(palette, waterSequence, true);

        /* setup raster callback */
        RasterCallback callback = new RasterCallback(MyRasterEffects);
        engine.SetRasterCallback(callback);

        /* main loop */
	    window = Window.Create(null, WindowFlags.Vsync);
	    while (window.Process ())
	    {
            /* inputs */
		    if (window.GetInput (Input.Right))
		    {
			    speed += 0.02f;
			    if (speed > 1.0f)
				    speed = 1.0f;
		    }
		    else if (speed > 0.0f)
		    {
			    speed -= 0.02f;
			    if (speed < 0.0f)
				    speed = 0.0f;
		    }
    			 
		    if (window.GetInput (Input.Left))
		    {
			    speed -= 0.02f;
			    if (speed < -1.0f)
				    speed = -1.0f;
		    }
		    else if (speed < 0.0f)
		    {
			    speed += 0.02f;
			    if (speed > 0.0f)
				    speed = 0.0f;
		    }

		    /* scroll */
		    posForeground += 3*speed;
            foreground.SetPosition((int)posForeground,0);
            for (c = 0; c < BackgroundStrips; c++)
			    posBackground[c] += (incBackground[c] * speed);

		    /* render to window */
            window.DrawFrame(frame);
		    frame++;
	    }

	    /* deinit */
        sp.Delete();
        engine.Deinit();

	    return 0;
    }

    /* raster effects (virtual HBLANK) */
    static void MyRasterEffects(int line)
    {
	    float pos =- 1;

	    if (line==1)
		    pos = posBackground[0];
	    else if (line==32)
		    pos = posBackground[1];
	    else if (line==48)
		    pos = posBackground[2];
	    else if (line==64)
		    pos = posBackground[3];
	    else if (line==112)
		    pos = posBackground[4];
	    else if (line >= 152)
		    pos = lerp (line, 152,224, (int)posBackground[4], (int)posBackground[5]);

	    if (pos != -1)
		    background.SetPosition((int)pos, 0);
    	
	    /* background color gradients */
	    if (line < 112)
            engine.SetBackgroundColor (InterpolateColor(line, 0, 112, skyColor[0], skyColor[1]));
	    else if (line >= 144)
            engine.SetBackgroundColor (InterpolateColor(line, 144, Vres, waterColor[0], waterColor[1]));
    }

    /* integer linear interploation */
    static int lerp (int x, int x0,int x1, int fx0, int fx1)
    {
	    return fx0 + (fx1 - fx0)*(x - x0)/(x1 - x0);
    }

    /* color interpolation */
    static Color InterpolateColor(int v, int v1, int v2, Color color1, Color color2)
    {
        return new Color(
            (byte)lerp(v, v1, v2, color1.R, color2.R),
            (byte)lerp(v, v1, v2, color1.G, color2.G),
            (byte)lerp(v, v1, v2, color1.B, color2.B)
        );
    }
}
