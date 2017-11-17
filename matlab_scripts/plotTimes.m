%Generates a time-plot for each method, shows the plots and saves them
%to the specified time_plot_path (set in init.m)
for j =1:5
    figure('Name',methodNames(j))
    grid on
    for i=1:9
        yyaxis right
        if (i ~= 9)
            plot(N, AAA(i, :, j), 'color', cc(i,:), 'linewidth', 1.5, 'marker', ch(i))
        else
            plot(N, q_m, 'color', [0, 0, 0], 'linewidth', 1.5, 'marker', ch(9))
        end
        yyaxis left
        if (i ~= 9)
            plot(N, AAA(i, :, j), 'color', cc(i,:), 'linewidth', 1.5, 'marker', ch(i))
        else
            plot(N, q_m, 'color', [0, 0, 0], 'linewidth', 1.5, 'marker', ch(9))
        end
        hold on;
    end
    %plot(N, q_m, 'color', [0, 0, 0], 'linewidth', 2, 'marker', ch(9))
    title(sprintf('Time Comparison of Bitonic (%s) with\n 2^p threads and qsort on 2^q element arrays', methodNames(j)))
    ylabel('Median runtime on diades (s)')
    xlabel('q')
    lgd = legend({'1', '2', '3', '4', '5', '6', '7', '8', 'qsort'},'Location','northwest');
    title(lgd, 'p')
    print([time_plot_path  char(methodNames(j)) ' time'], '-dpng')
end