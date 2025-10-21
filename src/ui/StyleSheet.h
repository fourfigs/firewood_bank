#pragma once

#include <QString>

namespace FirewoodStyles {

// ==================== COLOR PALETTE ====================

// Primary Colors (Nature + Forest)
const QString FOREST_GREEN = "#2d5016";
const QString DEEP_PINE = "#1a2f0c";
const QString WARM_EARTH = "#8b6f47";
const QString WOOD_BROWN = "#5c4033";

// Fiery Accents
const QString EMBER_ORANGE = "#d97732";
const QString FLAME_RED = "#c1421e";
const QString GOLDEN_GLOW = "#f4a300";

// Neutral Tones
const QString SMOKE_WHITE = "#f8f9fa";
const QString ASH_GRAY = "#6b7280";
const QString CHARCOAL = "#1f2937";
const QString CREAM = "#fef6e4";

// Status Colors
const QString SUCCESS_GREEN = "#28a745";
const QString WARNING_AMBER = "#ffc107";
const QString INFO_BLUE = "#0078d4";
const QString DANGER_RED = "#dc3545";

// ==================== GRADIENTS ====================

// Fiery gradient for headers and important elements
const QString GRADIENT_EMBER_TO_FLAME = 
    "qlineargradient(x1:0, y1:0, x2:1, y2:0, "
    "stop:0 #d97732, stop:0.3 #c1421e, stop:0.7 #c1421e, stop:1 #d97732)";

// Forest gradient for toolbars and navigation
const QString GRADIENT_FOREST = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 #2d5016, stop:1 #1a2f0c)";

// Button gradient for primary actions (enhanced)
const QString GRADIENT_PRIMARY_BUTTON = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 #e88842, stop:0.5 #d97732, stop:1 #c1421e)";

// Button gradient for hover
const QString GRADIENT_PRIMARY_BUTTON_HOVER = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 #f59952, stop:0.5 #e88842, stop:1 #d97732)";

// Active tab gradient
const QString GRADIENT_ACTIVE_TAB = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 #d97732, stop:1 #f4a300)";

// Success gradient
const QString GRADIENT_SUCCESS = 
    "qlineargradient(x1:0, y1:0, x2:1, y2:0, "
    "stop:0 #28a745, stop:1 #20c997)";

// Warning gradient  
const QString GRADIENT_WARNING = 
    "qlineargradient(x1:0, y1:0, x2:1, y2:0, "
    "stop:0 #ffc107, stop:1 #ff9800)";

// ==================== COMPONENT STYLES ====================

// Main Window Background
const QString MAIN_WINDOW = 
    "QMainWindow { "
    "   background-color: " + SMOKE_WHITE + "; "
    "} "
    "QMainWindow::separator { "
    "   background: " + ASH_GRAY + "; "
    "   width: 1px; "
    "   height: 1px; "
    "}";

// Toolbar with forest gradient and smooth interactions
const QString TOOLBAR = 
    "QToolBar { "
    "   background: " + GRADIENT_FOREST + "; "
    "   border: none; "
    "   border-bottom: 3px solid " + EMBER_ORANGE + "; "
    "   padding: 8px; "
    "   spacing: 10px; "
    "} "
    "QToolBar::separator { "
    "   background: " + WARM_EARTH + "; "
    "   width: 3px; "
    "   margin: 8px 6px; "
    "} "
    "QToolButton { "
    "   background: transparent; "
    "   border: 2px solid transparent; "
    "   color: white; "
    "   padding: 8px 16px; "
    "   border-radius: 6px; "
    "   font-size: 11pt; "
    "   font-weight: 500; "
    "} "
    "QToolButton:hover { "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " + EMBER_ORANGE + ", stop:1 " + FLAME_RED + "); "
    "   border: 2px solid " + GOLDEN_GLOW + "; "
    "   color: white; "
    "   font-weight: bold; "
    "} "
    "QToolButton:pressed { "
    "   background: " + FLAME_RED + "; "
    "   border: 2px solid " + FLAME_RED + "; "
    "   padding: 9px 16px 7px 16px; "
    "}";

// Primary Button (fiery gradient with smooth animations)
const QString PRIMARY_BUTTON = 
    "QPushButton.primary { "
    "   background: " + GRADIENT_PRIMARY_BUTTON + "; "
    "   color: white; "
    "   border: 2px solid transparent; "
    "   border-radius: 8px; "
    "   padding: 12px 24px; "
    "   font-weight: bold; "
    "   font-size: 11pt; "
    "   min-width: 120px; "
    "   min-height: 40px; "
    "} "
    "QPushButton.primary:hover { "
    "   background: " + GRADIENT_PRIMARY_BUTTON_HOVER + "; "
    "   border: 2px solid " + GOLDEN_GLOW + "; "
    "   padding: 12px 24px; "
    "   transform: scale(1.05); "
    "} "
    "QPushButton.primary:pressed { "
    "   background: " + FLAME_RED + "; "
    "   padding: 13px 24px 11px 24px; "
    "   border: 2px solid " + FLAME_RED + "; "
    "} "
    "QPushButton.primary:disabled { "
    "   background: " + ASH_GRAY + "; "
    "}";

// Secondary Button (earthy tones)
const QString SECONDARY_BUTTON = 
    "QPushButton.secondary { "
    "   background: " + WARM_EARTH + "; "
    "   color: white; "
    "   border: none; "
    "   border-radius: 5px; "
    "   padding: 10px 20px; "
    "   font-size: 10pt; "
    "   min-width: 100px; "
    "} "
    "QPushButton.secondary:hover { "
    "   background: " + WOOD_BROWN + "; "
    "} "
    "QPushButton.secondary:pressed { "
    "   background: " + DEEP_PINE + "; "
    "}";

// Cancel/Danger Button
const QString CANCEL_BUTTON = 
    "QPushButton.cancel { "
    "   background: white; "
    "   color: " + CHARCOAL + "; "
    "   border: 2px solid " + ASH_GRAY + "; "
    "   border-radius: 5px; "
    "   padding: 10px 20px; "
    "   font-size: 10pt; "
    "   min-width: 100px; "
    "} "
    "QPushButton.cancel:hover { "
    "   border-color: " + FLAME_RED + "; "
    "   color: " + FLAME_RED + "; "
    "} "
    "QPushButton.cancel:pressed { "
    "   background: " + CREAM + "; "
    "}";

// Table/TableView with warm tones
const QString TABLE_VIEW = 
    "QTableView { "
    "   background-color: white; "
    "   alternate-background-color: " + CREAM + "; "
    "   selection-background-color: " + EMBER_ORANGE + "; "
    "   selection-color: white; "
    "   gridline-color: #e0d4c8; "
    "   border: 1px solid " + WARM_EARTH + "; "
    "   border-radius: 4px; "
    "} "
    "QTableView::item { "
    "   padding: 6px; "
    "} "
    "QTableView::item:hover { "
    "   background-color: #ffe8d6; "
    "} "
    "QHeaderView::section { "
    "   background: " + GRADIENT_FOREST + "; "
    "   color: white; "
    "   padding: 8px; "
    "   border: none; "
    "   border-right: 1px solid " + WARM_EARTH + "; "
    "   font-weight: bold; "
    "   font-size: 9pt; "
    "}";

// GroupBox with context colors - ENHANCED FOR READABILITY
const QString GROUP_BOX = 
    "QGroupBox { "
    "   border: 2px solid " + WARM_EARTH + "; "
    "   border-radius: 10px; "
    "   margin-top: 20px; "
    "   padding: 22px 18px 18px 18px; "
    "   font-weight: bold; "
    "   font-size: 11pt; "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 white, stop:1 #fafafa); "
    "} "
    "QGroupBox::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 8px 16px; "
    "   background: " + GRADIENT_WARNING + "; "
    "   color: " + CHARCOAL + "; "
    "   border-radius: 6px; "
    "   left: 15px; "
    "   font-size: 11pt; "
    "   font-weight: bold; "
    "}";

// Client/Success themed GroupBox - ENHANCED FOR READABILITY
const QString GROUP_BOX_SUCCESS = 
    "QGroupBox.success { "
    "   border: 2px solid " + SUCCESS_GREEN + "; "
    "   border-radius: 10px; "
    "   margin-top: 20px; "
    "   padding: 22px 18px 18px 18px; "
    "   font-weight: bold; "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f0fdf4, stop:1 #e6f9ed); "
    "} "
    "QGroupBox.success::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 6px 14px; "
    "   background-color: " + SUCCESS_GREEN + "; "
    "   color: white; "
    "   border-radius: 5px; "
    "   left: 12px; "
    "   font-size: 11pt; "
    "}";

// Delivery/Fire themed GroupBox - ENHANCED FOR READABILITY
const QString GROUP_BOX_FIRE = 
    "QGroupBox.fire { "
    "   border: 2px solid " + EMBER_ORANGE + "; "
    "   border-radius: 8px; "
    "   margin-top: 18px; "
    "   padding: 20px 15px 15px 15px; "
    "   font-weight: bold; "
    "   background-color: " + CREAM + "; "
    "} "
    "QGroupBox.fire::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 6px 14px; "
    "   background-color: " + EMBER_ORANGE + "; "
    "   color: white; "
    "   border-radius: 5px; "
    "   left: 12px; "
    "   font-size: 11pt; "
    "}";

// Urgent/Priority themed GroupBox - ENHANCED FOR READABILITY
const QString GROUP_BOX_URGENT = 
    "QGroupBox.urgent { "
    "   border: 2px solid " + FLAME_RED + "; "
    "   border-radius: 8px; "
    "   margin-top: 18px; "
    "   padding: 20px 15px 15px 15px; "
    "   font-weight: bold; "
    "   background-color: #fef2f2; "
    "} "
    "QGroupBox.urgent::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 6px 14px; "
    "   background-color: " + FLAME_RED + "; "
    "   color: white; "
    "   border-radius: 5px; "
    "   left: 12px; "
    "   font-size: 11pt; "
    "}";

// Tab Widget with wood tones
const QString TAB_WIDGET = 
    "QTabWidget::pane { "
    "   border: 2px solid " + WARM_EARTH + "; "
    "   border-radius: 4px; "
    "   background: white; "
    "   top: -2px; "
    "} "
    "QTabBar::tab { "
    "   background: " + WARM_EARTH + "; "
    "   color: white; "
    "   padding: 10px 20px; "
    "   margin-right: 4px; "
    "   border-top-left-radius: 4px; "
    "   border-top-right-radius: 4px; "
    "   font-size: 10pt; "
    "   min-width: 100px; "
    "} "
    "QTabBar::tab:hover { "
    "   background: " + EMBER_ORANGE + "; "
    "} "
    "QTabBar::tab:selected { "
    "   background: " + GRADIENT_ACTIVE_TAB + "; "
    "   font-weight: bold; "
    "} "
    "QTabBar::tab:!selected { "
    "   margin-top: 4px; "
    "}";

// Line Edit with warm border - ENHANCED FOR READABILITY
const QString LINE_EDIT = 
    "QLineEdit { "
    "   border: 2px solid " + ASH_GRAY + "; "
    "   border-radius: 5px; "
    "   padding: 8px 12px; "
    "   background: white; "
    "   color: " + CHARCOAL + "; "
    "   font-size: 11pt; "
    "   min-height: 20px; "
    "} "
    "QLineEdit:focus { "
    "   border-color: " + EMBER_ORANGE + "; "
    "   border-width: 2px; "
    "} "
    "QLineEdit:disabled { "
    "   background: " + SMOKE_WHITE + "; "
    "   color: " + ASH_GRAY + "; "
    "} "
    "QTextEdit { "
    "   border: 2px solid " + ASH_GRAY + "; "
    "   border-radius: 5px; "
    "   padding: 8px 12px; "
    "   background: white; "
    "   color: " + CHARCOAL + "; "
    "   font-size: 11pt; "
    "} "
    "QTextEdit:focus { "
    "   border-color: " + EMBER_ORANGE + "; "
    "}";

// ComboBox with forest theme - ENHANCED FOR READABILITY
const QString COMBO_BOX = 
    "QComboBox { "
    "   border: 2px solid " + ASH_GRAY + "; "
    "   border-radius: 5px; "
    "   padding: 8px 12px; "
    "   background: white; "
    "   color: " + CHARCOAL + "; "
    "   font-size: 11pt; "
    "   min-width: 120px; "
    "   min-height: 20px; "
    "} "
    "QComboBox:focus { "
    "   border-color: " + EMBER_ORANGE + "; "
    "} "
    "QComboBox::drop-down { "
    "   border: none; "
    "   background: " + WARM_EARTH + "; "
    "   width: 32px; "
    "   border-top-right-radius: 4px; "
    "   border-bottom-right-radius: 4px; "
    "} "
    "QComboBox::down-arrow { "
    "   image: none; "
    "   border-left: 5px solid transparent; "
    "   border-right: 5px solid transparent; "
    "   border-top: 7px solid white; "
    "   margin-right: 10px; "
    "} "
    "QComboBox QAbstractItemView { "
    "   border: 2px solid " + WARM_EARTH + "; "
    "   selection-background-color: " + EMBER_ORANGE + "; "
    "   selection-color: white; "
    "   background: white; "
    "   color: " + CHARCOAL + "; "
    "   font-size: 11pt; "
    "   padding: 4px; "
    "} "
    "QSpinBox, QDoubleSpinBox, QDateEdit { "
    "   border: 2px solid " + ASH_GRAY + "; "
    "   border-radius: 5px; "
    "   padding: 8px 12px; "
    "   background: white; "
    "   color: " + CHARCOAL + "; "
    "   font-size: 11pt; "
    "   min-height: 20px; "
    "} "
    "QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus { "
    "   border-color: " + EMBER_ORANGE + "; "
    "}";

// Label styles - ENHANCED FOR READABILITY
const QString LABEL_HEADER = 
    "QLabel.header { "
    "   font-size: 15pt; "
    "   font-weight: bold; "
    "   color: " + FOREST_GREEN + "; "
    "   padding: 10px 0px; "
    "}";

const QString LABEL_SUBHEADER = 
    "QLabel.subheader { "
    "   font-size: 12pt; "
    "   font-weight: bold; "
    "   color: " + CHARCOAL + "; "
    "   padding: 6px 0px; "
    "}";

// Form labels
const QString FORM_LABELS = 
    "QLabel { "
    "   color: " + CHARCOAL + "; "
    "   font-size: 10pt; "
    "   padding: 2px 0px; "
    "}";

// Text Edit style (for multi-line text input)
const QString TEXT_EDIT = 
    "QTextEdit { "
    "   border: 2px solid " + ASH_GRAY + "; "
    "   border-radius: 5px; "
    "   padding: 8px 12px; "
    "   background: white; "
    "   color: " + CHARCOAL + "; "
    "   font-size: 11pt; "
    "   min-height: 60px; "
    "} "
    "QTextEdit:focus { "
    "   border-color: " + EMBER_ORANGE + "; "
    "   border-width: 2px; "
    "} "
    "QTextEdit:disabled { "
    "   background: " + SMOKE_WHITE + "; "
    "   color: " + ASH_GRAY + "; "
    "}";

// Organization Header (for WorkOrderDialog)
const QString ORG_HEADER = 
    "QLabel.orgHeader { "
    "   background: " + GRADIENT_EMBER_TO_FLAME + "; "
    "   color: white; "
    "   border-radius: 6px; "
    "   padding: 15px 20px; "
    "   font-size: 16pt; "
    "   font-weight: bold; "
    "}";

// ==================== COMBINED STYLESHEET ====================

// Complete application stylesheet
const QString APPLICATION_STYLE = 
    MAIN_WINDOW + 
    TOOLBAR + 
    PRIMARY_BUTTON + 
    SECONDARY_BUTTON + 
    CANCEL_BUTTON + 
    TABLE_VIEW + 
    GROUP_BOX + 
    GROUP_BOX_SUCCESS + 
    GROUP_BOX_FIRE + 
    GROUP_BOX_URGENT + 
    TAB_WIDGET + 
    LINE_EDIT + 
    TEXT_EDIT + 
    COMBO_BOX + 
    LABEL_HEADER + 
    LABEL_SUBHEADER + 
    FORM_LABELS + 
    ORG_HEADER;

} // namespace FirewoodStyles

