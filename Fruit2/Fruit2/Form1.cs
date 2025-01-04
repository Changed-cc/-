using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Fruit2
{
    public partial class Form1 : Form
    {
        //启动标志
        public static bool start_flag = false;
        //互斥信号量，实现对盘子的互斥访问
        public static Mutex mutex = new Mutex();
        //同步信号量
        public static Semaphore fruit_empty, fruit_full, apple_empty, apple_full, orange_empty, orange_full;
        public static int apple_num = 0;
        public static int orange_num = 0;
        public static int fruit_num = 0;



        int LplateX = 350;
        int LplateY = 300;
        int RplateX = 400;
        int RplateY = 300;

        public Form1()
        {
            InitializeComponent();
            Control.CheckForIllegalCrossThreadCalls = false;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            apple1.Visible = false;
            orange1.Visible = false;
            apple2.Visible = false;
            orange2.Visible = false;
            //设置最多允许有两个线程同时访问该信号量，且初始时该信号量的计数为2。
            // 初始化信号量
            fruit_empty = new Semaphore(2, 2);  // 盘子最多可容纳 2 个水果，初始化为可容纳 2 个
            fruit_full = new Semaphore(0, 2);   // 盘子最开始为空，因此满的信号量初始为 0
            apple_empty = new Semaphore(2, 2);  // 最开始没有苹果，信号量为 2，表示可放置
            apple_full = new Semaphore(0, 2);   // 没有苹果时，表示已放置的苹果数为 0
            orange_empty = new Semaphore(2, 2); // 最开始没有橘子，信号量为 2，表示可放置
            orange_full = new Semaphore(0, 2);  // 没有橘子时，表示已放置的橘子数为 0

            Task fatherTask = Task.Run(() => Father());
            Task motherTask = Task.Run(() => Mother());
            Task sonTask = Task.Run(() => Son());
            Task daughterTask = Task.Run(() => Daughter());


            

        }

        public void Father()
        {
            while (true)  // 生产者应一直尝试生产
            {
                // 盘子必须有空位才能放水果
                fruit_empty.WaitOne();
                fruit_empty.WaitOne();
                // 苹果位置必须有空位
                apple_empty.WaitOne();
                apple_empty.WaitOne();
                // 加锁仅用于对共享变量的操作
                mutex.WaitOne();
                // 执行手部动画，放苹果
                F_hand_move(2);
                // 增加苹果数量
                apple_num+=2;
                fruit_num+=2;
                // 释放锁
                mutex.ReleaseMutex();
                apple_full.Release();
                apple_full.Release();
                fruit_full.Release();
                fruit_full.Release();
            }
        }

        public void Mother()
        {
            while (true)  // 生产者应一直尝试生产
            {
                fruit_empty.WaitOne();    // 盘子必须有空位才能放水果
                orange_empty.WaitOne();   // 必须有空位放橘子
                mutex.WaitOne();
                M_hand_move(1);  // 执行手部动画，放橘子
                orange_num++;
                fruit_num++;
                mutex.ReleaseMutex();     // 释放互斥锁
                orange_full.Release();    // 增加一个橘子
                fruit_full.Release();     // 盘子增加一个水果
            }
        }

        public void Son()
        {
            while (true)  // 消费者应一直尝试消费
            {
                fruit_full.WaitOne();  // 等待盘子里有水果
                orange_full.WaitOne(); // 等待有橘子
                mutex.WaitOne();  // 加锁，确保对共享变量的安全操作
                S_hand_move();  // 执行手部动画，取橘子
                orange_num--;
                fruit_num--;  // 减少盘子中水果的数量
                mutex.ReleaseMutex();  // 释放互斥锁
                orange_empty.Release(); // 通知生产者橘子位置空出来了
                fruit_empty.Release();  // 通知盘子有空位了
            }
        }

        public void Daughter()
        {
            while (true)  // 消费者应一直尝试消费
            {
                fruit_full.WaitOne();  // 等待盘子里有水果
                apple_full.WaitOne();  // 等待有苹果
                mutex.WaitOne();  // 加锁，确保对共享变量的安全操作
                D_hand_move();  // 执行手部动画，取苹果
                D_hand_move();
                apple_num--;
                fruit_num--;  // 减少盘子中水果的数量
                mutex.ReleaseMutex();  // 释放互斥锁
                apple_empty.Release(); // 通知生产者苹果位置空出来了
                fruit_empty.Release(); // 通知盘子有空位了
            }
        }


        // 爸爸的手的动画
        public void F_hand_move(int num)
        {
            int x = 187, y = 53;  // 爸爸的手的初始位置
            int n = 10;

            
            for (int i = 0; i < num; i++)
            {
                if (apple1.Visible == false && orange1.Visible == false)
                {
                    AnimateHandMovement(father_hand, apple1, x, y, 25, 20, n);
                }
                else if (apple2.Visible == false && orange2.Visible == false)
                {
                    AnimateHandMovement(father_hand, apple2, x, y, 30, 20, n);
                }
                else
                {
                    MessageBox.Show("盘子放满了！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }
            }
        }

        // 妈妈的手的动画
        public void M_hand_move(int num)
        {
            int x = 600, y = 53;
            int n = 10;

           
            for (int i = 0; i < num; i++)
            {
                if (apple1.Visible == false && orange1.Visible == false)
                {
                    AnimateHandMovement(mother_hand, orange1, x, y, -30, 20, n);
                }
                else if (apple2.Visible == false && orange2.Visible == false)
                {
                    AnimateHandMovement(mother_hand, orange2, x, y, -25, 20, n);
                }
                else
                {
                    MessageBox.Show("盘子放满了！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }
            }
        }

        //女儿的手的动画
        public void D_hand_move()
        {
            int x = daughter_hand.Location.X, y = daughter_hand.Location.Y;
            int n = 10;
            if (apple1.Visible == true)
            {
                for (int i = 0; i < n; i++)
                {
                    daughter_hand.Location = new Point(x, y);
                    x += 21;
                    y -= 8;
                    Thread.Sleep(100);
                }
                for (int i = 0; i < n; i++)
                {
                    x -= 21;
                    y += 8;
                    apple1.Location = new Point(x, y);
                    daughter_hand.Location = new Point(x, y);
                    Thread.Sleep(100);
                }
                apple1.Visible = false;
            }
            else if (apple2.Visible == true)
            {
                for (int i = 0; i < n; i++)
                {
                    daughter_hand.Location = new Point(x, y);
                    x += 15;
                    y -= 8;
                    Thread.Sleep(100);
                }
                for (int i = 0; i < n; i++)
                {
                    x -= 15;
                    y += 8;
                    apple2.Location = new Point(x, y);
                    daughter_hand.Location = new Point(x, y);
                    Thread.Sleep(100);
                }
                apple2.Visible = false;
            }
            else
            {
                for (int i = 0; i < n; i++)
                {
                    daughter_hand.Location = new Point(x, y);
                    x += 12;
                    y -= 8;
                    Thread.Sleep(100);
                }
                for (int i = 0; i < n; i++)
                {
                    x -= 12;
                    y += 8;
                    daughter_hand.Location = new Point(x, y);
                    Thread.Sleep(100);
                }
            }
        }

        public void AnimateHandMovement(Control hand, Control fruit, int startX, int startY, int deltaX, int deltaY, int steps)
        {
            int x = startX;
            int y = startY;

            for (int i = 0; i < steps; i++)
            {
                hand.Location = new Point(x, y);
                fruit.Visible = true;
                fruit.Location = new Point(x, y);
                x += deltaX;
                y += deltaY;
                Thread.Sleep(100);
            }
            for (int i = 0; i < steps; i++)
            {
                x -= deltaX;
                y -= deltaY;
                hand.Location = new Point(x, y);
                Thread.Sleep(100);
            }
        }

        //儿子的手的动画
        public void S_hand_move()
        {
            int x = son_hand.Location.X, y = son_hand.Location.Y;
            int n = 10;
            if (orange1.Visible == true)    //盘子左边有橘子
            {
                for (int i = 0; i < n; i++)
                {
                    son_hand.Location = new Point(x, y);
                    x -= 15;
                    y -= 8;
                    Thread.Sleep(100);
                }
                for (int i = 0; i < n; i++)
                {
                    x += 15;
                    y += 8;
                    son_hand.Location = new Point(x, y);
                    orange1.Visible = true;
                    orange1.Location = new Point(x, y);
                    Thread.Sleep(100);
                }
                orange1.Visible = false;
            }
            else if (orange2.Visible == true)   //盘子右边有橘子
            {
                for (int i = 0; i < n; i++)
                {
                    son_hand.Location = new Point(x, y);
                    x -= 21;
                    y -= 8;
                    Thread.Sleep(100);
                }
                for (int i = 0; i < n; i++)
                {
                    x += 21;
                    y += 8;
                    son_hand.Location = new Point(x, y);
                    orange2.Location = new Point(x, y);
                    Thread.Sleep(100);
                }
                orange2.Visible = false;
            }
            else if (orange1.Visible == false && orange2.Visible == false)    //盘中无橘子
            {
                for (int i = 0; i < n; i++)
                {
                    son_hand.Location = new Point(x, y);
                    x -= 12;
                    y -= 8;
                    Thread.Sleep(100);
                }
                for (int i = 0; i < n; i++)
                {
                    x += 12;
                    y += 8;
                    son_hand.Location = new Point(x, y);
                    Thread.Sleep(100);
                }
            }
        }
        private void Stop_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);

        }
    }
}
