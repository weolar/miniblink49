#ifndef BASE_METRICS_HISTOGRAM_H_
#define BASE_METRICS_HISTOGRAM_H_

#define UMA_HISTOGRAM_CUSTOM_TIMES(name, duration, time, time2, time3)
#define UMA_HISTOGRAM_CUSTOM_COUNTS(name, a, b, c, d)
#define UMA_HISTOGRAM_COUNTS(name, a)
#define UMA_HISTOGRAM_COUNTS_100(name, a)
#define UMA_HISTOGRAM_MEMORY_MB(name, a)
#define UMA_HISTOGRAM_BOOLEAN(name, a)
#define UMA_HISTOGRAM_TIMES(name, a)
#define UMA_HISTOGRAM_ENUMERATION(name, a, b)
#define UMA_HISTOGRAM_SPARSE_SLOWLY(name, a)
#define SCOPED_UMA_HISTOGRAM_TIMER(name)
#endif // BASE_METRICS_HISTOGRAM_H_