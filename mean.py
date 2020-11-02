import cv2

img_a = cv2.imread("radiance-A.exr", cv2.IMREAD_UNCHANGED)
img_b = cv2.imread("radiance-B.exr", cv2.IMREAD_UNCHANGED)

img_mean = (img_a + img_b) / 2
cv2.imwrite("radiance-mean.exr", img_mean)
