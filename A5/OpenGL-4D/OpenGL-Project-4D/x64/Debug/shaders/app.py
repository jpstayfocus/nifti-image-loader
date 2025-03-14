import nibabel as nib
import nilearn as nil
import scipy.ndimage as ndi
import matplotlib.pyplot as plt
import os

brain_vol = nib.load('t1.nii')

# What is the type of this object?
type(brain_vol)


print(brain_vol.header)

brain_vol_data = brain_vol.get_fdata()
type(brain_vol_data)

# print(brain_vol_data.shape)
print(brain_vol_data[96].size)
plt.imshow(ndi.rotate(brain_vol_data[96], 90), cmap='bone')
plt.axis('off')
plt.show()