d:
cd D:\NewMain\Universitaet\Das_Letzte_Jahr_ist_Over\Vertiefung\Diffusion\
ffmpeg -f image2 -framerate 50 -i D:\NewMain\Universitaet\Das_Letzte_Jahr_ist_Over\Vertiefung\Diffusion\Bilder\img%%d.bmp -s 1920x1080 -c:v libx264 -crf 10 -pixel_format rgb24 D:\NewMain\Universitaet\Das_Letzte_Jahr_ist_Over\Vertiefung\Diffusion\Diffusion.mp4
