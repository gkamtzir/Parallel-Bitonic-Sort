%Compares the best case scenario times of each method for every array size
%it plots the results (saves the plot in the plot_path specified folder).
colormap(cc);
hb = bar(N, M,'FaceColor', 'flat');
grid on
barWidth = hb.BarWidth;
numCol = size(M, 2);
cnt = 0;
for ii = numbersToAdd'
    cnt = cnt + 1;
    xPos = linspace(cnt - barWidth/2, cnt + barWidth / 2, numCol+1);
    idx = 1;
    for jj = xPos(1:end-1)
        val = numbersToAdd(cnt,idx);
        y = M(cnt,idx);
        if (val ~= 0)
            %val = 2^val;
            text(jj+15.05, y +0.05, num2str(val))
        end
        idx = idx +1;
    end     
end

%lgd = legend({'pthreads', 'cilk recursive', 'cilk imperative', 'omp recursive', 'omp imperative',  'qsort'},'Location','northwest');
lgd = legend(methodNames,'Location','northwest');
title(lgd, 'Methods')
set(gca, 'fontsize', 22);
title(sprintf('Time Comparison of the different methods on 2^q element arrays\n and optimal number of threads 2^p (p is shown over the bars)'))
ylabel('Median runtime on diades')
xlabel('q')

fig = gcf;
fig.Units = 'normalized';
fig.OuterPosition = [0 0 1 1];

print(fig, [plot_path 'method_comparison'], '-dpng');