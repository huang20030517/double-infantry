# 硬件

## 开发板信息

[kit/DM-MC02](https://gitee.com/kit-miao/dm-mc02)

---

## CAN 电机 ID 分配

### 底盘  (CAN1 )

- **FIFO0**
    - M0 :  ***0x201***（前左)
    - M1 :  **0x202**（前右)
    - M2 :  ***0x203***（后左)
    - M3 :  ***0x204***（后右)
- **FIFO1**
    - Yaw  :  **0x209**
    - 超电：**0x210**

---

### 左头 (CAN2)

- **FIFO0**
    - 供弹电机 : **0x205**
    - 摩擦轮
        - 左电机：**0x206**
        - 右电机：**0x207**
    - Yaw：0x208
- **FIFO1**
    - pitch：0x141

---

### 右头 (CAN3)

- 供弹电机 : **0x205**
- 摩擦轮
    - 左电机：**0x206**
    - 右电机：**0x207**
- Yaw：0x208
- pitch ：0x141
