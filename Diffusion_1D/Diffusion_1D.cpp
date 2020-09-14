// Diffusion_1D.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <vector>
#include <bitmap_image.hpp>
#include<ppl.h>



void Farbpalette_Blau_zu_Rot_256(std::vector<rgb_t>& Farbpalette)
{
    Farbpalette.resize(256);
    for (size_t i = 0; i <= 63; i++)
    {
        Farbpalette[i] = make_colour(0, 4 * i, 255);
        Farbpalette[64 + i] = make_colour(0, 255, 255 - 4 * i);
        Farbpalette[128 + i] = make_colour(4 * i, 255, 0);
        Farbpalette[192 + i] = make_colour(255, 255 - 4 * i, 0);
    }

}


bitmap_image Erzeuge_Heatmap_BMP(std::vector<rgb_t>& Farbpalette,std::vector<double>&Gitter, std::vector<double>& Laenge, double LaengePerStep)
{

    int x = Gitter.size(),k=100;
    bitmap_image Output_Image(x, k);

    std::vector<double> Positionen;
    double Summe=0;
    for (int i = 0; i < Laenge.size() - 1; i++)
    {
        Summe += Laenge[i];
        Positionen.push_back(Summe);
        //std::cout << Positionen[i] << std::endl;
    }
   

    //Steigung m berechnen
    double Range_Faktor = (1.0 - 0.0) / (Farbpalette.size() - 1);

    for (size_t i = 0; i <= k - 1; i++)
    {
        for (size_t j = 0; j <= x - 1; j++)
        {
            int Farbe;
            Farbe = int(round((Gitter[j] - 0.0) / Range_Faktor));

            if (Farbe > Farbpalette.size() - 1)
            {
                Farbe = Farbpalette.size() - 1;
            }
            else if (Farbe < 0)
            {
                Farbe = 0;
            }
            
            

            Output_Image.set_pixel(j, i, Farbpalette[Farbe]);

            for (int g = 0; g < Positionen.size(); g++)
            {
                if (j == int(Positionen[g]/ LaengePerStep))
                {
                    Output_Image.set_pixel(j, i, 255, 255, 255);//Weiß
                }

                //std::cout << int(Positionen[g] / LaengePerStep) << std::endl;
            }

            if (j == Gitter.size()-101) //TCD-Position
            {
                Output_Image.set_pixel(j, i, 128, 128, 128);//Grau
            }


                if (i == int(round(Gitter[j] * k)))
                {
                    Output_Image.set_pixel(j, i, 0,0,0);//Schwarz
                }

        }
    }

    return Output_Image;
}

void Speichere_BMPs(std::vector<bitmap_image>& Bild_Dateien, std::string Speicherordner)
{
    std::cout << "Speichere " + Speicherordner + "\n";

    for (int i = 0; i <= Bild_Dateien.size() - 1; i++)
    {
        Bild_Dateien[i].save_image( Speicherordner + "\\img" + std::to_string(i + 1) + ".bmp");
    }


}

void Erzeuge_Output_Videos(std::string& ffmpeg_Dir, std::string Bilder)
{
    std::string command_cd_ffmpeg = "cd " + ffmpeg_Dir;

    std::string command_movie_front = "ffmpeg -f image2 -framerate 50 -i ";
    std::string command_movie_middle = "img%%d.bmp -s 1920x1080 -c:v libx264 -crf 10 -pixel_format rgb24 ";

    std::string command_data_Potential = Bilder+"\\";

    std::string command_out_Potential = ffmpeg_Dir + "\\Diffusion.mp4";
 

    std::string command_create_Potential_movie = command_movie_front + command_data_Potential + command_movie_middle + command_out_Potential;
   

    std::fstream Ouput;

    Ouput.open("ffmpeg.bat", std::ios::out);
    Ouput <<ffmpeg_Dir[0] <<":\n";
    Ouput << command_cd_ffmpeg + "\n";
    Ouput << command_create_Potential_movie + "\n";
   
    Ouput.close();

    system("ffmpeg.bat");
}



int main()
{
    //Finite-Differenzen Solver für 1D Diffusion auf einem Gitter 
    //Expansion auf 2D später?

    /*Plan:     
    
    Erstellen eines diskreten Gitters der Flussbedingungen in 1D in einem Rohr mit Fluss
    Erstellen einer Heatmap der Ergebnisse
    Erstellen eines Videos mit Legende (0-100% Volumenanteil/Konzentration)
    
    
    */

    //std::vector<std::vector<double>> Konzentration;


    std::vector<double> Gitter,Gitter_dt,Gitter_0,TCD,Flussgeschwindigkeit,Durchmesser,Laenge;


    double Fluss;
    std::cout << "Eingabe Volumenfluss[mL/min]:";
    std::cin >> Fluss;

    double Pulsdauer=0;
    char Input_Modus='a';
    
    std::cout << "Eingabe Eingangsmodus: Rechteck(r)\tGauss(g)\n";
    std::cin >> Input_Modus;
    std::cout << std::endl;

    if (Input_Modus == 'r')
    {
        std::cout << "Eingabe Pulsdauer[s]:";
        std::cin >> Pulsdauer;
        std::cout << std::endl;
    }
    else if (Input_Modus == 'g')
    {
        std::cout << "Eingabe Reduktionszeit[s]:";
        std::cin >> Pulsdauer;
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Fehlerhafte Auswahl\n\nProgramm beenden";
        return -1;
    }
    



    int nAbschnitte;
    std::cout << "Eingabe Anzahl Abschnitte:";
    std::cin >> nAbschnitte;
    std::cout << std::endl;

    Durchmesser.resize(nAbschnitte, 0);
    Laenge.resize(nAbschnitte, 0);
    double Gesamtlange = 0;


    for (int i = 0; i < nAbschnitte; i++)
    {
        
        std::cout << "Eingabe Laenge[cm] Abschnitt " << i+1 <<":";
        std::cin >> Laenge[i];
        Laenge[i] = Laenge[i] / 100;
        Gesamtlange += Laenge[i];

        std::cout << "Eingabe Durchmesser[mm] Abschnitt " << i+1 << ":";
        std::cin >> Durchmesser[i];
        std::cout << std::endl;
             
        
    }
            
        
    double Diffusionskoeffizient = 1.7e-5;//[m^2/s]
        
    double LaengePerStep = 0.001; //[m] Gute Laenge für aktuelles Problem für kleineren Diff vielleicht kleinere Laenge

    double Timestep = 0.0002;   //[s] Guter Timestep aktuell, vielleicht kleiner für schnelleren Fluss?
    
    double t_max = 15*60;//Sekunden

    int Gittergroesse = int(Gesamtlange / LaengePerStep);


    for (int i = 0; i < nAbschnitte; i++)
    {
        for (int j = 0; j < Laenge[i]/LaengePerStep; j++)
        {
            double Flaeche = Durchmesser[i] * Durchmesser[i] * 3.1415 / 4; //Durchmesser in mm -> Faktor 10^-6 | ml->m^3 Faktor 10^-6

            Flussgeschwindigkeit.push_back((Fluss / 60) / Flaeche);
        }
    }
    
    //std::cout << Flussgeschwindigkeit.size() << std::endl << Gittergroesse;


    Gitter.resize(Gittergroesse, 0);
    Gitter_dt = Gitter;
    Gitter_0 = Gitter;
       


    std::vector<rgb_t> Farbpalette;
    Farbpalette_Blau_zu_Rot_256(Farbpalette);


    std::vector<bitmap_image> Images;
    Images.reserve(10000);
      
    std::string  Ordner_Name;
    std::cout << "Eingabe Bilder Ordner:";
    std::cin >> Ordner_Name;
    std::cout << std::endl;

    std::string ffmpeg_dir;
    std::cout << "Eingabe ffmpeg Ordner:";
    std::cin >> ffmpeg_dir;
    std::cout << std::endl;

    //std::string  Ordner_Name = "D:\\NewMain\\Universitaet\\Das_Letzte_Jahr_ist_Over\\Vertiefung\\Diffusion\\Bilder", ffmpeg_dir = "D:\\NewMain\\Universitaet\\Das_Letzte_Jahr_ist_Over\\Vertiefung\\Diffusion\\";
    
    int g = 0;

    for (int t = 0; t < t_max/Timestep; t++)
    {

        if(t%100==0)
        { 
        Images.push_back(Erzeuge_Heatmap_BMP(Farbpalette, Gitter,Laenge, LaengePerStep));

        Images[g].save_image(Ordner_Name + "\\img" + std::to_string(g + 1) + ".bmp");
        g++;
        TCD.push_back(Gitter[Gittergroesse - 100]);
        }

        if (t % 5000 == 0)
        {
            std::cout << "Simulation:" << t/5000 << " Sekunden\t" << t << " | " << int(t_max / Timestep) << std::endl;
        }

        //Gitter = Konzentration[Konzentration.size() - 1];
    
       //Randbedingung
        if (Input_Modus == 'r')
        {
            if (t * Timestep < Pulsdauer)
            {
                Gitter[0] = 1.0;
                Gitter_dt[0] = 1.0;
            }
            else
            {
                Gitter[0] = 0.0;
                Gitter_dt[0] = 0.0;
            }
        }
        else//Pulsdauer = Reduktionszeit bei Gauß
        {
            if (t * Timestep < Pulsdauer)
            {
                Gitter[0] = 1.0 * exp(-0.5 * (t * Timestep - Pulsdauer * 0.5) * (t * Timestep - Pulsdauer * 0.5) / (Pulsdauer * 0.125 * Pulsdauer * 0.125));
                Gitter_dt[0] = 1.0 * exp(-0.5 * ((t + 1) * Timestep - Pulsdauer * 0.5) * ((t + 1) * Timestep - Pulsdauer * 0.5) / (Pulsdauer * 0.125 * Pulsdauer * 0.125));
            }
            else
            {
                Gitter[0] = 0.0;
                Gitter_dt[0] = 0.0;
            }
        }
        

        int Ende = 0;


        //Concurrency::parallel_for(1, Gittergroesse - 2, [](int& i,std::vector<double> Gitter, std::vector<double> Gitter_dt, std::vector<double> Flussgeschwindigkeit,double LaengePerStep,double Diffusionskoeffizient, double Timestep,int Ende)
        //    {


        //        Gitter_dt[i] = Timestep * (Diffusionskoeffizient * (Gitter[i + 1] + Gitter[i - 1] - 2.0 * Gitter[i]) / (LaengePerStep * LaengePerStep) - Flussgeschwindigkeit[i] * (Gitter[i + 1] - Gitter[i - 1]) / (2 * LaengePerStep)) + Gitter[i];


        //        if (Gitter_dt[i] < 1.0e-4) //1.0e-4 Ist ca. die Messschwelle des TCD
        //        {
        //            Ende++;
        //        }

        //    });


    for (int i = 1; i < Gittergroesse - 2; i++)
    {
        Gitter_dt[i] = Timestep * (Diffusionskoeffizient * (Gitter[i + 1] + Gitter[i - 1] - 2.0 * Gitter[i]) / (LaengePerStep * LaengePerStep) - Flussgeschwindigkeit[i] * (Gitter[i + 1] - Gitter[i - 1]) / (2 * LaengePerStep))+ Gitter[i];
        
        
        if (Gitter_dt[i] < 1.0e-4) //1.0e-4 Ist ca. die Messschwelle des TCD
        {
            Ende++;
        }
    }
    
    //Gitter_dt[x-1] = Timestep * (Diffusionskoeffizient * (2.0*Gitter[x-2] - 2.0 * Gitter[x - 1]) / (LaengePerStep * LaengePerStep) ) + Gitter[x-1];

    //Gitter_dt[x - 1] = Timestep * (Diffusionskoeffizient * (Konzentration[Konzentration.size() - 1][x-1]+ Gitter[x - 2] - 2.0 * Gitter[x - 1]) / (LaengePerStep * LaengePerStep) - v * (Gitter[x - 1] - Gitter[x - 2]) / LaengePerStep) + Gitter[x - 1];

    Gitter_dt[Gittergroesse - 1] = 0;
    //Gitter[0] -> Quelle Randbedingung
    //Gitter[x-1] -> Neumann -> glattes Ende des Simulationsgebietes

    //Konzentration.push_back(Gitter_dt);
    Gitter = Gitter_dt;
    Gitter_dt = Gitter_0;
    
    //std::cout << Gitter[1] << std::endl;

    if (Ende == Gittergroesse - 4 && t*Timestep>10)
    {
        break;
    }
    

    }
    
    std::fstream Datei;

    Datei.open(ffmpeg_dir + "\\TCD.txt", std::ios::out);

    Datei << "Zeit [s]\tVolumenanteil [%/100]" << std::endl;

    for(int i= 0;i!= TCD.size() - 1;i++)
    { 
        Datei << i*0.02 << "\t" << TCD[i]<<std::endl;

    }

    Datei.close();

    
   

    //Speichere_BMPs(Images, Ordner_Name);

    Erzeuge_Output_Videos(ffmpeg_dir,Ordner_Name);


}

// Programm ausführen: STRG+F5 oder "Debuggen" > Menü "Ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
