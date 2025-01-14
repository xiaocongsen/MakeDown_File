# 将行中多个日期数据扩展为多行，并添加月份和日期列，并排除总计行

``` py
# -*- coding: utf-8 -*-
import pandas as pd

# 读取Excel文件
df = pd.read_excel('your_file.xlsx')

# 将多列数据转换为长格式（每个日期占一行）
df_melted = df.melt(id_vars='登记人', var_name='日期', value_name='值')

# 替换 '值' 列中的 NaN 值为 0 (使用推荐的方法)
df_melted.fillna({'值': 0}, inplace=True)

# 尝试将日期列转换为 datetime 类型，并处理不能转换的值（将其设置为 NaT）
df_melted['日期'] = pd.to_datetime(df_melted['日期'], errors='coerce')

# 过滤掉非日期行（即日期为 NaT 的行）
df_melted.dropna(subset=['日期'], inplace=True)

# 提取月份并添加到 DataFrame 中，保持月份为数字形式
df_melted['月份'] = df_melted['日期'].dt.month

# 格式化日期列为 'YYYY-MM-DD' 格式
df_melted['日期'] = df_melted['日期'].dt.strftime('%Y-%m-%d')

# 排除姓名中包含“总计”的行
df_melted = df_melted[~df_melted['登记人'].str.contains('总计', na=False)]

# 按照字段姓名和日期排序
df_sorted = df_melted.sort_values(by=['登记人', '日期'])

# 调整列顺序，将月份列移动到日期和值之间
df_sorted = df_sorted[['登记人', '日期', '月份', '值']]

# 更改列名
df_sorted.rename(columns={'登记人': '姓名', '值': '工时'}, inplace=True)

# 重置索引
df_sorted.reset_index(drop=True, inplace=True)

# 如果需要保存到新的Excel文件
df_sorted.to_excel('expanded_data.xlsx', index=False)

print("Data processing finished.")
```

# 一个Excel有多个工作表，且格式一致，将其合并成一个新的Excel文件，并筛选出指定部门的数据

```py
# -*- coding: utf-8 -*-
import pandas as pd

# 假设您的Excel文件路径为file.xlsx，且所有sheet的结构相同
excel_file = 'E:\\2024.xlsx'

# 定义部门过滤条件
conditions = [
    '平台技术', 'Python组', '客户端组', 'Web前端组',
    'Web后台组（长沙）1', 'Web后台组（长沙）2', 'Web后台组（SDP组）',
    'Web后台组（深圳）', '后台C++组（DLP组）', '后台C++组（NXG组）',
    '后台C++组（SDP组）', '后台C++组（终端）', '后台C++组（准入）',
    '客户端C++组（DES组）', '客户端C++组（EDR/AV组）', '客户端C++组（SDP组）',
    '客户端C++组（基础组）', '客户端C++组（网络组）', '客户端C++组（文件组）',
    '客户端C++组（质量组）'
]

# 读取Excel文件的所有工作表到一个字典中，键为sheet名，值为DataFrame
workbook = pd.read_excel(excel_file, sheet_name=None)

# 初始化一个空的DataFrame用于存储筛选后的数据
filtered_data = []

# 遍历每个工作表并应用过滤条件
for sheet_name, df in workbook.items():
    # 筛选出部门为指定条件的数据
    filtered_df = df[df['部门'].isin(conditions)]
    # 将筛选出的数据添加到列表中
    filtered_data.append(filtered_df)

# 合并所有筛选后的数据到一个大的DataFrame
merged_df = pd.concat(filtered_data, ignore_index=True)

# 按照字段姓名和日期排序
merged_df.sort_values(by=['姓名', '日期'], inplace=True)

# 写入新的Excel文件
output_file = 'E:\\merged_attendance_data.xlsx'
merged_df.to_excel(output_file, index=False)

print("Attendance Merged finished.")
```

# 将两个Excel文件合并，并筛选出指定字段，并计算两个字段的差值，并添加一个新列

```py
# -*- coding: utf-8 -*-
import pandas as pd

df1 = pd.read_excel('E:\\merged_attendance_data.xlsx',engine='openpyxl')
df2 = pd.read_excel('E:\\expanded_data.xlsx',engine='openpyxl')

df1['日期'] = df1['日期'].replace('', pd.NaT)  # 将空字符串替换为Not a Time (NaT)
df1['日期'] = pd.to_datetime(df1['日期'], errors='coerce', format='%Y-%m-%d')  # 使用errors='coerce'来处理不能转换为日期的值，它们将被设置为NaT
df1['日期'] = df1['日期'].dt.strftime('%Y-%m-%d')  # 转换为'年-月-日'格式
df1['日期'] = df1['日期'].str.strip()  # 去除前后空格


df2['日期'] = df2['日期'].replace('', pd.NaT)  # 将空字符串替换为Not a Time (NaT)
df2['日期'] = pd.to_datetime(df2['日期'], errors='coerce', format='%Y-%m-%d')  # 使用errors='coerce'来处理不能转换为日期的值，它们将被设置为NaT
df2['日期'] = df2['日期'].dt.strftime('%Y-%m-%d')  # 转换为'年-月-日'格式
df2['日期'] = df2['日期'].str.strip()  # 去除前后空格

merged_df = pd.merge(df1, df2, on=['姓名', '日期'], how='inner')
output_columns = ['姓名', '日期', '部门']

merged_df = merged_df[output_columns]

merged_df = pd.merge(df1, df2, on=['姓名', '日期'], how='inner')
output_columns = ['姓名', '日期', '部门','工时','实际出勤时长（研发）']

merged_df = merged_df[output_columns]
merged_df['工时差'] =  merged_df['工时'] - merged_df['实际出勤时长（研发）']

# 添加“是否合规”列
def is_compliant(row):
    if row['实际出勤时长（研发）'] == 0:
        if row['工时'] != 0:
            return ''
        else:
            return '合规'
    elif row['实际出勤时长（研发）'] >= 7 and 7.5 <= row['工时'] <= 12:
        return '合规'
    elif row['实际出勤时长（研发）'] < 7:
        if row['工时'] != 0:
            if row['工时'] > 12:
                return '不合规'
            else:
                return ''
        else:
            return '不合规'
    else:
        return '不合规'

merged_df['是否合规'] = merged_df.apply(is_compliant, axis=1)
# 更改列名
merged_df.rename(columns={'部门': '小组', '工时': '禅道工时'}, inplace=True)

# 排查实际出勤时长（研发）为0的内容
# zero_hours = merged_df[merged_df['实际出勤时长（研发）'] != 0]



merged_df.to_excel('E:\\matched_and_merge.xlsx', index=False)

print("Merged finished.")
```

```py
# -*- coding: utf-8 -*-
import pandas as pd

df1 = pd.read_excel('E:\\merged_attendance_data.xlsx',engine='openpyxl')
df2 = pd.read_excel('E:\\expanded_data.xlsx',engine='openpyxl')

df1['日期'] = df1['日期'].replace('', pd.NaT)  # 将空字符串替换为Not a Time (NaT)
df1['日期'] = pd.to_datetime(df1['日期'], errors='coerce', format='%Y-%m-%d')  # 使用errors='coerce'来处理不能转换为日期的值，它们将被设置为NaT
df1['日期'] = df1['日期'].dt.strftime('%Y-%m-%d')  # 转换为'年-月-日'格式
df1['日期'] = df1['日期'].str.strip()  # 去除前后空格


df2['日期'] = df2['日期'].replace('', pd.NaT)  # 将空字符串替换为Not a Time (NaT)
df2['日期'] = pd.to_datetime(df2['日期'], errors='coerce', format='%Y-%m-%d')  # 使用errors='coerce'来处理不能转换为日期的值，它们将被设置为NaT
df2['日期'] = df2['日期'].dt.strftime('%Y-%m-%d')  # 转换为'年-月-日'格式
df2['日期'] = df2['日期'].str.strip()  # 去除前后空格

merged_df = pd.merge(df1, df2, on=['姓名', '日期'], how='inner')
output_columns = ['姓名', '日期', '部门']

merged_df = merged_df[output_columns]

#selected_columns = ['姓名', '日期', '最早卡', '最早卡','工时']
#merged_df = pd.merge(df1[['id', '日期', '工时']], df2[['姓名', 'location']], on=['id'])
#merged_df = merged_df[selected_columns]

merged_df = pd.merge(df1, df2, on=['姓名', '日期'], how='inner')
output_columns = ['姓名', '日期', '月份','部门','工时','实际出勤时长（研发）']

merged_df = merged_df[output_columns]
merged_df['工时差'] =  merged_df['工时'] - merged_df['实际出勤时长（研发）']

# 按照姓名和日期进行分组，并对工时求和
grouped_df = merged_df.groupby(['姓名','部门','月份']).agg({'工时': 'sum', '实际出勤时长（研发）': 'sum', '工时差': 'sum'}).reset_index()

#grouped_df['工时偏差率'] = (grouped_df['工时差'] / grouped_df['实际出勤时长（研发）']) * 100
# 如果需要确保结果始终为正数（偏差率为正或负），可以先取绝对值再格式化
#grouped_df['工时偏差率'] = grouped_df['工时偏差率'].apply(lambda x: f"{abs(x):.2f}%" if x != float('inf') else '0.00%')
grouped_df['工时偏差率'] = grouped_df.apply(lambda row: '0.00%' if row['实际出勤时长（研发）'] == 0 else f"{(row['工时差'] / row['实际出勤时长（研发）']) * 100:.2f}%", axis=1)
#grouped_df['工时偏差率']的绝对值
grouped_df['是否达标'] = grouped_df['工时偏差率'].apply(lambda x: f"{abs(float(x.strip('%'))):.2f}%")
grouped_df['是否达标'] = grouped_df['是否达标'].apply(lambda x: '达标' if float(x.strip('%')) <= 10 else '不达标')

# 更改列名
grouped_df.rename(columns={'部门': '小组', '工时': '禅道工时'}, inplace=True)

grouped_df.to_excel('E:\\matched_and_merge.xlsx', index=False)

print("Merged finished.")
```