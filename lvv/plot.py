import pandas as pd
import matplotlib.pyplot as plt

def plot_csv_functions(filename):
    df = pd.read_csv(filename)

    required_columns = ['x', 'vectime', 'listtime', 'vecgain']
    if not all(column in df.columns for column in required_columns):
        missing_columns = [col for col in required_columns if col not in df.columns]
        print(f"Error: The CSV file is missing the following columns: {', '.join(missing_columns)}")
        return

    window_size = 2000

    # Calculate rolling averages
    df['rolling_avg_vectime'] = df['vectime'].rolling(window=window_size).mean()
    df['rolling_avg_listtime'] = df['listtime'].rolling(window=window_size).mean()
    df['rolling_avg_vecgain'] = df['vecgain'].rolling(window=window_size).mean()

    # Plotting
    plt.plot(df['x'], df['vectime'], label='std::vec time', color='blue')
    plt.plot(df['x'], df['listtime'], label='std::list time', color='orange')
    plt.plot(df['x'], df['vecgain'], label='Speedup using std::vec', color='green')
    plt.plot(df['x'], df['rolling_avg_vectime'], label=f'{window_size}-Rolling Avg of std::vec time', linestyle='--', color = 'blue')
    plt.plot(df['x'], df['rolling_avg_listtime'], label=f'{window_size}-Rolling Avg of std::list time', linestyle='--', color = 'orange')
    plt.plot(df['x'], df['rolling_avg_vecgain'], label=f'{window_size}-Rolling Avg of Speedup', linestyle='--', color = 'green')

    plt.xlabel('Number of Elements')
    plt.ylabel('Time (ns)')
    plt.title('List vs. Vector Performance Comparison')
    plt.legend()
    plt.grid(True)
    plt.show()

plot_csv_functions('lvv/out.csv')
