%Generates the speedup plots in relation to std's qsort (one plot for each
%method) and saves them to (the specified in init.m) speedUp_plot_path.
for j =1:5
    figure('Name',methodNames(j))
    for i=1:8
        ac = q_m ./ AAA(i, :, j);
        grid on
        yyaxis right
        plot(N, ac, 'color', cc(i,:), 'linewidth', 2, 'marker', ch(i))
        yyaxis left
        plot(N, ac, 'color', cc(i,:), 'linewidth', 2, 'marker', ch(i))
        
        hold on;
    end
    title(sprintf('Bitonic(%s) vs qsort\n comparative diagram with 2^p threads and 2^q elements', methodNames(j)))
    ylabel('qsort_{time}/'+methodNames(j)+'_{time}')
    xlabel('q')
    %'Location',  'best' is an option too
    lgd = legend({'1', '2', '3', '4', '5', '6', '7', '8'},'Location','northwest');
    title(lgd, 'p')
    print([speedUp_plot_path  char(methodNames(j)) ' speedUp'], '-dpng')
end

