using System;

class Test
{
    static int Main(string[] args)
    {
        bool ok;
        IntPtr bitmap;
        TLN.SpriteInfo info = new TLN.SpriteInfo();

        ok = TLN.Init(640, 480, 0, 0, 0);
        ok = TLN.CreateWindow("overlay.bmp", TLN.WindowFlags.CWF_VSYNC);
        bitmap = TLN.LoadBitmap("cycles\\v16pm.png");
        ok = TLN.SetBGBitmap(bitmap);

        IntPtr spriteset = TLN.LoadSpriteset("simon");
        IntPtr spriteset2 = TLN.CloneSpriteset(spriteset);

        TLN.Rect[] rects = new TLN.Rect[4];
        IntPtr spr = TLN.CreateSpriteset(4, rects, new byte[320], 320, 200, 320, IntPtr.Zero);

        ok = TLN.GetSpriteInfo(spriteset, 0, ref info);
        ok = TLN.GetSpriteInfo(spriteset2, 3, ref info);

        /* main loop */
        while (TLN.ProcessWindow())
        {
            DrawFrame(0);
            //TLN.DrawFrame(0);
        }

        TLN.DeleteWindow();
        TLN.Deinit();

        return 0;
    }

    /* draw frame line by line */
    static void DrawFrame(int frame)
    {
        int line = 0;

        TLN.BeginWindowFrame(frame);
        while (TLN.DrawNextScanline())
            DoRasterEffects(line++);
        TLN.EndWindowFrame();
    }

    static void DoRasterEffects(int line)
    {
    }
}
