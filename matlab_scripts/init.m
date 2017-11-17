%Initializes the environment for running the comparison scripts.
%!!!assumes that current directory is matlab_scripts!!!

%---INPUT FILE PATHS FOR LINUX/OS and WINDOWS---
data_file_name = 'data.txt';
data_paths = ["../data/" "..\data\"];

%---OUTPUT FILE PATHS FOR LINUX/OS and WINDOWS---
plot_paths = ["../plots/" "../plots/times/" "../plots/speedUps/" ... 
    "..\plots\" "..\plots\times\" "..\plots\speedUps\"];
text_paths = ["./" ".\"];

if ispc
    data_path = char(data_paths(2));
    plot_path = char(plot_paths(4));
    time_plot_path = char(plot_paths(5));
    speedUp_plot_path = char(plot_paths(6));
    text_path = char(text_paths(2));
else
    data_path = char(data_paths(1));
    plot_path = char(plot_paths(1));
    time_plot_path = char(plot_paths(2));
    speedUp_plot_path = char(plot_paths(3));
    text_path = char(text_paths(1));
end
%------------------------------------------------

edit raw_run_times.m;
copyfile([data_path data_file_name], 'raw_run_times.m' );

raw_run_times
run_times_medians

N=16:24;
N_extra = 25:29;

threads = [pt1_m; pt2_m; pt3_m; pt4_m; pt5_m; pt6_m; pt7_m; pt8_m];
omprs = [ompr1_m; ompr2_m; ompr3_m; ompr4_m; ompr5_m; ompr6_m; ompr7_m; ompr8_m];
ompis = [ompi1_m; ompi2_m; ompi3_m; ompi4_m; ompi5_m; ompi6_m; ompi7_m; ompi8_m];
crs = [cr1_m; cr2_m; cr3_m; cr4_m; cr5_m; cr6_m; cr7_m; cr8_m];
cis = [ci1_m; ci2_m; ci3_m; ci4_m; ci5_m; ci6_m; ci7_m; ci8_m];

AAA = threads;
AAA(:, :, 2) = crs;
AAA(:, :, 3) = cis;
AAA(:, :, 4) = omprs;
AAA(:, :, 5) = ompis;

[Mt, It] = min(threads);
[Mcr, Icr] = min(crs);
[Mci, Ici] = min(cis);
[Mompr, Iompr] = min(omprs);
[Mompi, Iompi] = min(ompis);

M = [Mt', Mcr', Mci', Mompr', Mompi', q_m'];
I = [It', Icr', Ici', Iompr', Iompi'];
numbersToAdd = [It', Icr', Ici', Iompr', Iompi', zeros(1, 9)'];

%---Names for the graphs---
methodNames = ["Pthreads recursive"; "Cilk recursive"; "Cilk imperative"; "OpenMP recursive"; "OpenMP imperative"; "qsort"];

%---Colors for the graphs---
cc = [
        230, 25, 75;
        60, 180, 75;
        240, 50, 230;
        0, 130, 200;
        245, 130, 48;
        145, 30, 180;
        70, 240, 240;
        210, 245, 60;
        255, 225, 25
    ] / 255;
%---Markers for the graphs---
ch = ['o', '+', 'p', '*', 's', 'x', '^', 'h', '.'];
