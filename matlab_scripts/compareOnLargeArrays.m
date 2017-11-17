%This script compares the (8 threaded) pthreads implementation 
%and qsort on array sizes up to 2^29
%generates 2 plots and saves them on plot_path (specified in init.m)

%---Time Comparison---
figure ('Name', 'Pthreads Recursive')
hold on
grid on
plot([N N_extra], [pt3_m pt3_extra_m], ...
    'color', cc(2, :), 'linewidth', 1.5, 'marker', 'p')
plot([N N_extra], [q_m q_extra_m], ...
    'color', [0, 0, 0], 'linewidth', 1.5, 'marker', 'o')
title(sprintf('Time Comparison of Bitonic (8 threads on 8 core cpu)\n and qsort on 2^q element arrays'))
    ylabel('Median runtime on diades (s)')
    xlabel('q')
    lgd = legend({'Pthreads Recursive', 'qsort'},'Location','northwest');
print([plot_path 'pthreads_big_array_time'],'-dpng')
%---Phtreads speed up---
figure ('Name', 'Pthreads Recursive')

plot([N N_extra], [q_m q_extra_m]./[pt3_m pt3_extra_m], ...
    'color', cc(2, :), 'linewidth', 1.5, 'marker', 'p')
title(sprintf('Bitonic (pthreads recursive with 8 threads on 8 core cpu)\n vs qsort on 2^q element arrays'))
    ylabel('qsort_{time}/pthreads_{time}')
    xlabel('q')
    grid on
print([plot_path 'pthreads_big_array_speedUp'],'-dpng')
