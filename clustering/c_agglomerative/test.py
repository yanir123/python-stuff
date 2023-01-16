import numpy as np

a = np.array([[1, 2], [3, 4]])
b = np.array([[7, 9], [12, 13]])

c = np.concatenate((a, b))

angles = np.arctan2(np.diff(c[:, 0]), np.diff(c[:, 1])) * 180 / np.pi

print(f'var: {angles.var()}')