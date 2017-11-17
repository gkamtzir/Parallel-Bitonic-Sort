%Finds the best combination of the sort's implementation
%generates a file with time data, comparing the sorts
%and 2 plots comparing the combination's times vs qsort times
%plots are saved in the specified plot_path (see init.m).
%The FILE should be opened with a code editor or inside matlab 
%on windows to appear correctly (as it uses \n to add new lines)
fileID = fopen([text_path 'bestAlgo.txt'],'w');

[Mb, Ib] = min(M');

%---accelaration plot----
figure
plot(N, q_m ./ Mb , 'color', [230, 25, 75]./255, 'linewidth', 1.5, 'marker', 'p')
title(sprintf('Bitonic(Optimal Algorithm, 8 core cpu) vs qsort\n comparative diagram on 2^q element arrays'))
    ylabel('qsort_{time}/OptimalSort_{time} on diades')
    xlabel('q')
    grid on
print([plot_path 'best_sort_speed_up'],'-dpng')
%---time plot----
figure
plot(N, Mb, 'color', [230, 25, 75]./255, 'linewidth', 1.5, 'marker', 'p')
hold on
plot(N, q_m, 'color', [0, 0, 0], 'linewidth', 1.5, 'marker', 'o')
    title(sprintf('Time Comparison of Bitonic (Optimal Algorithm, 8 core cpu)\n and qsort on 2^q element arrays'))
    ylabel('Median runtime on diades (s)')
    xlabel('q')
    lgd = legend({'Optimal Sort', 'qsort'},'Location','northwest');
    grid on
print([plot_path 'best_sort_time_plot'],'-dpng')
%---Method best times comparison----
for i = 1:9
    fprintf(fileID, 'For 2^%d elements use %18s with 2^%d threads and estimated time=%f\n', i+15, methodNames(Ib(i)), I(i, Ib(i)), Mb(i));
    for j = 1:6
        fprintf(fileID, '---%18s ''s estimated time +%fs or +%4.2f%%\n', methodNames(j), (M(i, j) - Mb(i)), M(i, j)/Mb(i)*100 - 100);
    end
end
fclose(fileID);