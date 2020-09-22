G:
cd G:\Diffusion_Sim
ffmpeg -f image2 -framerate 50 -i G:\Diffusion_Sim\AS_Reaktor_Bild\img%%d.bmp -s 1920x1080 -c:v libx264 -crf 10 -pixel_format rgb24 G:\Diffusion_Sim\Diffusion.mp4
