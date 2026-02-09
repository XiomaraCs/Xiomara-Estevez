using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Dynamic_Calendar
{
    public partial class Form1 : Form
    {
        public static int _year, _month;
        public Form1()
        {
            InitializeComponent();

            // Ensure the form's Load event is wired so days are populated at startup
            this.Load += Form1_Load;

            // Ensure predictable layout for calendar tiles
            flowLayoutPanel1.FlowDirection = FlowDirection.LeftToRight;
            flowLayoutPanel1.WrapContents = true;
            flowLayoutPanel1.AutoScroll = true;
            flowLayoutPanel1.Padding = new Padding(0);
            flowLayoutPanel1.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;
            // Keep the right arrow close to the left arrow without changing the left arrow's
            // distance to the Month_label. Adjust at runtime so designer spacing to Month_label
            // remains unchanged.
            this.Shown += (s, e) =>
            {
                try
                {
                    // small gap between arrows
                    int arrowGap = 8;
                    pictureBox2.Left = pictureBox1.Right + arrowGap;
                }
                catch { }
            };
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            showDays(DateTime.Now.Month, DateTime.Now.Year);
        }

        private void showDays(int month, int year)
        {
            flowLayoutPanel1.Controls.Clear();
            _year = year;
            _month = month;

            string monthName = new DateTimeFormatInfo().GetMonthName(month);
            Month_label.Text = monthName.ToUpper() + " " + year;

            DateTime startOfTheMonth = new DateTime(year, month, 1);
            int daysInMonth = DateTime.DaysInMonth(year, month);

            // numeric DayOfWeek (0 = Sunday, 1 = Monday, ...)
            int offset = (int)startOfTheMonth.DayOfWeek;

            // add empty tiles to align first day under correct weekday
            for (int i = 0; i < offset; i++)
            {
                UcDay empty = new UcDay("");
                // ensure consistent sizing and spacing so wrapping is predictable
                empty.Size = new Size(173, 142);
                empty.Margin = new Padding(1);
                flowLayoutPanel1.Controls.Add(empty);
            }

            // add one tile per calendar day (1..daysInMonth)
            for (int d = 1; d <= daysInMonth; d++)
            {
                UcDay uc = new UcDay(d.ToString());
                uc.Size = new Size(173, 142);
                uc.Margin = new Padding(1);
                flowLayoutPanel1.Controls.Add(uc);
            }
            // Force layout pass so wrapping recalculates immediately
            flowLayoutPanel1.PerformLayout();
        }

        // Added to satisfy the Paint event wired in the designer (panel_3.Paint += panel2_Paint;)
        private void panel2_Paint(object sender, PaintEventArgs e)
        {
            // Intentionally left blank. Implement custom painting here if needed.
        }

        // left arrow
        private void pictureBox1_Click(object sender, EventArgs e)
        {
            _month -= 1;
            if (_month < 1)
            {
                _month = 12;
                _year -= 1;
            }
            showDays(_month, _year);
        }

        // right arrow
        private void pictureBox2_Click(object sender, EventArgs e)
        {
            _month += 1;
            if (_month > 12)
            {
                _month = 1;
                _year += 1;
            }
            showDays(_month, _year);
        }
    }
}
