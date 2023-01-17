import numpy as np
import datetime

a = np.array([[1, 2], [3, 4]])
b = np.array([[7, 9], [12, 13]])

start_time = datetime.datetime.now()
c = np.concatenate((a, b))
angles = np.arctan2(np.diff(c[:, 0]), np.diff(c[:, 1])) * 180 / np.pi
end_time = datetime.datetime.now()

print(f'var: {angles.var()}')
print(f'executed in: {(end_time - start_time).microseconds}')