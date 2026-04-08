import sys
import numpy as np
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget,
    QVBoxLayout, QHBoxLayout, QPushButton,
    QLabel, QFileDialog
)
from PyQt6.QtCore import Qt
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qtagg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure


class PlotCanvas(FigureCanvas):
    """Matplotlib canvas embedded in the Qt window."""

    def __init__(self, parent=None):
        self.fig = Figure(figsize=(8, 5), tight_layout=True)
        self.ax = self.fig.add_subplot(111)
        super().__init__(self.fig)
        self.setParent(parent)
        self._plot_placeholder()

    def _plot_placeholder(self):
        """Show a placeholder until real data is loaded."""
        x = np.linspace(0, 2 * np.pi, 1000)
        self.ax.plot(x, np.sin(x), color="#4C9BE8", linewidth=1.2)
        self.ax.set_title("Placeholder — load a CSV to plot real data")
        self.ax.set_xlabel("X")
        self.ax.set_ylabel("Y")
        self.ax.grid(True, alpha=0.3)
        self.draw()

    def plot_data(self, x, y, title="Data", xlabel="X", ylabel="Y"):
        """Plot x/y arrays. Handles large datasets fine."""
        self.ax.clear()
        self.ax.plot(x, y, linewidth=0.8, color="#4C9BE8")
        self.ax.set_title(title)
        self.ax.set_xlabel(xlabel)
        self.ax.set_ylabel(ylabel)
        self.ax.grid(True, alpha=0.3)
        self.draw()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt6 + Matplotlib")
        #self.resize(0, 0)

        # --- Central widget & layout ---
        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)
        layout.setContentsMargins(12, 12, 12, 12)
        layout.setSpacing(8)

        # --- Top bar: file selector + status label ---
        top_bar = QHBoxLayout()

        self.btn_open = QPushButton("Open CSV…")
        self.btn_open.setFixedWidth(120)
        self.btn_open.clicked.connect(self.open_file)
        top_bar.addWidget(self.btn_open)

        self.btn_plot = QPushButton("Plot")
        self.btn_plot.setFixedWidth(80)
        self.btn_plot.clicked.connect(self.plot)
        self.btn_plot.setEnabled(False)
        top_bar.addWidget(self.btn_plot)

        self.label_file = QLabel("No file selected")
        self.label_file.setAlignment(Qt.AlignmentFlag.AlignVCenter)
        top_bar.addWidget(self.label_file)

        top_bar.addStretch()
        layout.addLayout(top_bar)

        # --- Canvas + navigation toolbar ---
        self.canvas = PlotCanvas(self)
        self.toolbar = NavigationToolbar(self.canvas, self)

        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)

        # Internal state
        self._filepath = None
        self._data = None

    # ------------------------------------------------------------------
    def open_file(self):
        path, _ = QFileDialog.getOpenFileName(
            self, "Open CSV", "", "CSV files (*.csv);;All files (*)"
        )
        if not path:
            return
        self._filepath = path
        self.label_file.setText(path.split("/")[-1])  # show filename only

        # Load CSV — expects two columns: x, y
        try:
            self._data = np.loadtxt(path, delimiter=",", skiprows=1)
            self.btn_plot.setEnabled(True)
            self.label_file.setStyleSheet("color: green;")
        except Exception as e:
            self.label_file.setText(f"Error: {e}")
            self.label_file.setStyleSheet("color: red;")
            self._data = None
            self.btn_plot.setEnabled(False)

    def plot(self):
        if self._data is None:
            return
        # Support (N,) or (N, 2+) shaped arrays
        x = self._data[:, 0]
        y = self._data[:, 10]

        self.canvas.plot_data(
            x, y,
            title=self._filepath.split("/")[-1],
            xlabel="Column 0",
            ylabel="Column 1",
        )


# ----------------------------------------------------------------------
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())