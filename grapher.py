import os

starting_directory = '/opt/hadoop/home/hadoop3/results/'

def extract_time(group):
    to_graph_list =[]
    for graph_list in group:
        temp_list = []
        for path in graph_list:
            _path = starting_directory + path +'/'
            success_path = _path + '_SUCCESS'
            log_path = _path + 'run.log'
            total_time = 0
            if os.path.exists(success_path):
                # read file for info
                with open(log_path) as fp:  
                    for cnt, line in enumerate(fp):
                        line = line.strip()
                        if 'CPU time spent (ms)' in line:
                            total_time += int(line.split('=')[1])
            temp_list.append((path, total_time))
        if temp_list:
            to_graph_list.append(temp_list)
    return to_graph_list

if __name__ == "__main__":
    # get all subdirectories
    subdirectories =[x[0] for x in os.walk(starting_directory)]
    subdirectories = subdirectories[1:]

    #[[], [], ['i_67108864_2_1', 'i_67108864_2_4'], ['i_67108864_3_4', 'i_67108864_3_1']]
    correllated_group_16 = [[],[],[],[]]
    correllated_group_32 = [[],[],[],[]]
    correllated_group_64 = [[],[],[],[]]
    independent_group_16 = [[],[],[],[]]
    independent_group_32 = [[],[],[],[]]
    independent_group_64 = [[],[],[],[]]
    anticorrellated_group_16 = [[],[],[],[]]
    anticorrellated_group_32 = [[],[],[],[]]
    anticorrellated_group_64 = [[],[],[],[]]

    for path in subdirectories:
        # path = '/opt/hadoop/home/hadoop3/results/c_33554432_2_4'
        cut_path = path.strip(starting_directory)
        second_underscore = cut_path.index('_', cut_path.index('_') + 1)
        file_size = cut_path[second_underscore-2: second_underscore]
        reducer =int(cut_path[second_underscore+1: second_underscore+2])
        if 'c' == cut_path[0]:
          if file_size == '16':
            correllated_group_16[reducer].append(cut_path)
          elif file_size == '32':
            correllated_group_32[reducer].append(cut_path)
          else:
            correllated_group_64[reducer].append(cut_path)
        if 'a' == cut_path[0]:
          if file_size == '16':
            anticorrellated_group_16[reducer].append(cut_path)
          elif file_size == '32':
            anticorrellated_group_32[reducer].append(cut_path)
          else:
            anticorrellated_group_64[reducer].append(cut_path)
        if 'i' == cut_path[0]:
          if file_size == '16':
            independent_group_16[reducer].append(cut_path)
          elif file_size == '32':
            independent_group_32[reducer].append(cut_path)
          else:
            independent_group_64[reducer].append(cut_path)

    to_graph_dict = dict()
 
    to_graph_dict['correllated_group_16'] = extract_time(correllated_group_16)
    to_graph_dict['correllated_group_32'] = extract_time(correllated_group_32)
    to_graph_dict['correllated_group_64'] = extract_time(correllated_group_64)
    
    to_graph_dict['independent_group_16'] = extract_time(independent_group_16)
    to_graph_dict['independent_group_32'] = extract_time(independent_group_32)
    to_graph_dict['independent_group_64'] = extract_time(independent_group_64)

    to_graph_dict['anticorrellated_group_16'] = extract_time(anticorrellated_group_16)
    to_graph_dict['anticorrellated_group_32'] = extract_time(correllated_group_16)
    to_graph_dict['anticorrellated_group_64'] = extract_time(anticorrellated_group_64)
  




