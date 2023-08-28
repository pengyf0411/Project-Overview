import sensor, image, time
from pyb import UART
from pid import PID
from pyb import Servo

uart = UART(3, 115200)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # can be QVGA on M7...
sensor.skip_frames(30)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock() # Tracks FPS.

pan_servo=Servo(1)
tilt_servo=Servo(2)

pan_servo.calibration(500,2500,500)
tilt_servo.calibration(500,2500,500)

pan_pid = PID(p=0.07, i=0, imax=90) #脱机运行或者禁用图像传输，使用这个PID
tilt_pid = PID(p=0.05, i=0, imax=90) #脱机运行或者禁用图像传输，使用这个PID
#pan_pid = PID(p=0.1, i=0, imax=90)#在线调试使用这个PID
#tilt_pid = PID(p=0.1, i=0, imax=90)#在线调试使用这个PID

while(True):
    clock.tick()
    img = sensor.snapshot()
    img.lens_corr(1.8) #
    for code in img.find_qrcodes():
        #串口发送payload
        print(code.payload())
        FH= bytearray([0xb3,0xb3])
        uart.write(FH)
        uart.write(code.payload())
        FH = bytearray([0x0d,0x0a])
        uart.write(FH)
        time.sleep_ms(1000)

        frame_location = code.corners() #获取二维码四个顶点的坐标值
        x_location = (frame_location[0][0] + frame_location[2][0])/2 #通过1、3顶点间对角线中点确定x值
        y_location = (frame_location[1][1]+ frame_location[3][1])/2 #通过2、4顶点间对角线中点确定y值

        img.draw_cross(x_location,y_location)#在二维码中心画十字
        pan_error = img.width()/2-x_location #x轴方向上的偏差量
        tilt_error = img.height()/2-y_location #y轴方向上的偏差量

        pan_output=pan_pid.get_pid(pan_error,1)/2
        tilt_output=tilt_pid.get_pid(tilt_error,1)/2

        pan_servo.angle(pan_servo.angle()+pan_output) #输出x轴方向的PWM波控制云台追踪
        tilt_servo.angle(tilt_servo.angle()-tilt_output) #输出y轴方向的PWM波控制云台追踪

