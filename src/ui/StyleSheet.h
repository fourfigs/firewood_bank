#pragma once

#include <QString>

namespace AdobeStyles {

// ==================== NM-ERA INSPIRED WARM COLOR PALETTE ====================

// Primary NM-ERA Colors (Warm & Earthy)
const QString NMERA_PRIMARY = "#D2691E";        // Warm Orange (Firewood)
const QString NMERA_PRIMARY_DARK = "#B8860B";   // Dark Goldenrod
const QString NMERA_PRIMARY_LIGHT = "#F4A460";  // Sandy Brown
const QString NMERA_SECONDARY = "#8B4513";      // Saddle Brown
const QString NMERA_ACCENT = "#CD853F";         // Peru (Warm Brown)

// Adobe-inspired with NM-ERA warmth
const QString ADOBE_BLUE = "#1473E6";
const QString ADOBE_BLUE_DARK = "#0F5BB8";
const QString ADOBE_BLUE_LIGHT = "#4A90E2";

// Warm Background Palette
const QString DARK_BG = "#1A1A1A";              // Slightly warmer dark
const QString CARD_BG = "#2D2B28";              // Warm dark gray
const QString SURFACE_BG = "#3A3630";           // Warm surface
const QString BORDER_GRAY = "#4A453F";          // Warm border
const QString TEXT_PRIMARY = "#F5F5DC";         // Warm white (Beige)
const QString TEXT_SECONDARY = "#D2B48C";       // Tan
const QString TEXT_MUTED = "#A0A0A0";           // Muted gray

// Warm Accent Colors
const QString SUCCESS_GREEN = "#228B22";        // Forest Green
const QString WARNING_ORANGE = "#FF8C00";       // Dark Orange
const QString ERROR_RED = "#DC143C";            // Crimson
const QString INFO_CYAN = "#20B2AA";            // Light Sea Green
const QString FIRE_ORANGE = "#FF4500";          // Orange Red (Fire)
const QString WOOD_BROWN = "#8B4513";           // Saddle Brown

// Interactive States
const QString HOVER_BG = "#4A453F";
const QString ACTIVE_BG = "#5A554F";
const QString SELECTED_BG = NMERA_PRIMARY;

// ==================== FIREWOOD-THEMED ICONS ====================

// Firewood Bank Icons (Unicode + CSS)
const QString ICON_FIREWOOD = "🪵";
const QString ICON_FIRE = "🔥";
const QString ICON_TREE = "🌲";
const QString ICON_HOUSE = "🏠";
const QString ICON_TRUCK = "🚚";
const QString ICON_MONEY = "💰";
const QString ICON_USERS = "👥";
const QString ICON_INVENTORY = "📦";
const QString ICON_ORDERS = "📋";
const QString ICON_CALENDAR = "📅";
const QString ICON_CHART = "📊";
const QString ICON_SETTINGS = "⚙️";
const QString ICON_LOGOUT = "🚪";
const QString ICON_ADD = "➕";
const QString ICON_EDIT = "✏️";
const QString ICON_DELETE = "🗑️";
const QString ICON_SAVE = "💾";
const QString ICON_REFRESH = "🔄";
const QString ICON_SEARCH = "🔍";
const QString ICON_WARNING = "⚠️";
const QString ICON_SUCCESS = "✅";
const QString ICON_ERROR = "❌";
const QString ICON_INFO = "ℹ️";

// ==================== WARM GRADIENTS ====================

// NM-ERA Primary Gradient (Firewood Orange)
const QString GRADIENT_PRIMARY = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + NMERA_PRIMARY + ", stop:1 " + NMERA_PRIMARY_DARK + ")";

// Warm Card Gradient
const QString GRADIENT_CARD = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + CARD_BG + ", stop:1 " + SURFACE_BG + ")";

// Hover Gradient (Warm)
const QString GRADIENT_HOVER = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + HOVER_BG + ", stop:1 " + ACTIVE_BG + ")";

// Success Gradient (Forest Green)
const QString GRADIENT_SUCCESS = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + SUCCESS_GREEN + ", stop:1 #1E7B1E)";

// Warning Gradient (Fire Orange)
const QString GRADIENT_WARNING = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + WARNING_ORANGE + ", stop:1 " + FIRE_ORANGE + ")";

// Error Gradient (Crimson)
const QString GRADIENT_ERROR = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + ERROR_RED + ", stop:1 #B22222)";

// Fire Gradient (Special for firewood theme)
const QString GRADIENT_FIRE = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + FIRE_ORANGE + ", stop:1 " + NMERA_PRIMARY + ")";

// Wood Gradient (Wood brown theme)
const QString GRADIENT_WOOD = 
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, "
    "stop:0 " + WOOD_BROWN + ", stop:1 " + NMERA_SECONDARY + ")";

// ==================== MODERN COMPONENT STYLES ====================

// Main Window - Dark Adobe Theme
const QString MAIN_WINDOW = 
    "QMainWindow { "
    "   background-color: " + DARK_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QMainWindow::separator { "
    "   background: " + BORDER_GRAY + "; "
    "   width: 1px; "
    "   height: 1px; "
    "}";

// Modern Toolbar
const QString TOOLBAR = 
    "QToolBar { "
    "   background: " + CARD_BG + "; "
    "   border: none; "
    "   border-bottom: 1px solid " + BORDER_GRAY + "; "
    "   padding: 12px 16px; "
    "   spacing: 8px; "
    "} "
    "QToolBar::separator { "
    "   background: " + BORDER_GRAY + "; "
    "   width: 1px; "
    "   margin: 8px 4px; "
    "} "
    "QToolButton { "
    "   background: transparent; "
    "   border: none; "
    "   color: " + TEXT_SECONDARY + "; "
    "   padding: 8px 12px; "
    "   border-radius: 6px; "
    "   font-size: 13px; "
    "   font-weight: 500; "
    "   min-width: 32px; "
    "   min-height: 32px; "
    "} "
    "QToolButton:hover { "
    "   background: " + HOVER_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QToolButton:pressed { "
    "   background: " + ACTIVE_BG + "; "
    "   color: " + ADOBE_BLUE + "; "
    "} "
    "QToolButton:checked { "
    "   background: " + SELECTED_BG + "; "
    "   color: white; "
    "}";

// Modern Primary Button (NM-ERA Firewood Theme)
const QString PRIMARY_BUTTON = 
    "QPushButton.primary { "
    "   background: " + GRADIENT_PRIMARY + "; "
    "   color: white; "
    "   border: none; "
    "   border-radius: 10px; "
    "   padding: 14px 28px; "
    "   font-weight: 600; "
    "   font-size: 14px; "
    "   min-width: 140px; "
    "   min-height: 44px; "
    "   box-shadow: 0 2px 8px rgba(210, 105, 30, 0.3); "
    "   transition: all 0.2s ease; "
    "} "
    "QPushButton.primary:hover { "
    "   background: " + GRADIENT_FIRE + "; "
    "   transform: translateY(-1px); "
    "   box-shadow: 0 4px 12px rgba(210, 105, 30, 0.4); "
    "} "
    "QPushButton.primary:pressed { "
    "   background: " + NMERA_PRIMARY_DARK + "; "
    "   transform: translateY(0px); "
    "   box-shadow: 0 2px 6px rgba(210, 105, 30, 0.3); "
    "} "
    "QPushButton.primary:disabled { "
    "   background: " + BORDER_GRAY + "; "
    "   color: " + TEXT_MUTED + "; "
    "   transform: none; "
    "   box-shadow: none; "
    "}";

// Modern Secondary Button (Warm Theme)
const QString SECONDARY_BUTTON = 
    "QPushButton.secondary { "
    "   background: " + CARD_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   border: 2px solid " + BORDER_GRAY + "; "
    "   border-radius: 10px; "
    "   padding: 14px 28px; "
    "   font-weight: 500; "
    "   font-size: 14px; "
    "   min-width: 140px; "
    "   min-height: 44px; "
    "   transition: all 0.2s ease; "
    "} "
    "QPushButton.secondary:hover { "
    "   background: " + HOVER_BG + "; "
    "   border-color: " + NMERA_PRIMARY + "; "
    "   transform: translateY(-1px); "
    "   box-shadow: 0 2px 8px rgba(210, 105, 30, 0.2); "
    "} "
    "QPushButton.secondary:pressed { "
    "   background: " + ACTIVE_BG + "; "
    "   transform: translateY(0px); "
    "}";

// Modern Danger Button
const QString CANCEL_BUTTON = 
    "QPushButton.cancel { "
    "   background: " + CARD_BG + "; "
    "   color: " + ERROR_RED + "; "
    "   border: 1px solid " + ERROR_RED + "; "
    "   border-radius: 8px; "
    "   padding: 12px 24px; "
    "   font-weight: 500; "
    "   font-size: 14px; "
    "   min-width: 120px; "
    "   min-height: 40px; "
    "} "
    "QPushButton.cancel:hover { "
    "   background: " + ERROR_RED + "; "
    "   color: white; "
    "} "
    "QPushButton.cancel:pressed { "
    "   background: #DC2626; "
    "}";

// Modern Table View
const QString TABLE_VIEW = 
    "QTableView { "
    "   background-color: " + CARD_BG + "; "
    "   alternate-background-color: " + SURFACE_BG + "; "
    "   selection-background-color: " + SELECTED_BG + "; "
    "   selection-color: white; "
    "   gridline-color: " + BORDER_GRAY + "; "
    "   border: 1px solid " + BORDER_GRAY + "; "
    "   border-radius: 8px; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QTableView::item { "
    "   padding: 12px 8px; "
    "   border: none; "
    "} "
    "QTableView::item:hover { "
    "   background-color: " + HOVER_BG + "; "
    "} "
    "QTableView::item:selected { "
    "   background-color: " + SELECTED_BG + "; "
    "   color: white; "
    "} "
    "QHeaderView::section { "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   padding: 12px 8px; "
    "   border: none; "
    "   border-right: 1px solid " + BORDER_GRAY + "; "
    "   border-bottom: 1px solid " + BORDER_GRAY + "; "
    "   font-weight: 600; "
    "   font-size: 13px; "
    "}";

// Modern Card/GroupBox
const QString GROUP_BOX = 
    "QGroupBox { "
    "   border: 1px solid " + BORDER_GRAY + "; "
    "   border-radius: 12px; "
    "   margin-top: 16px; "
    "   padding: 20px; "
    "   font-weight: 600; "
    "   font-size: 14px; "
    "   background: " + CARD_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QGroupBox::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 8px 16px; "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   border-radius: 8px; "
    "   left: 16px; "
    "   font-size: 14px; "
    "   font-weight: 600; "
    "}";

// Success Card
const QString GROUP_BOX_SUCCESS = 
    "QGroupBox.success { "
    "   border: 1px solid " + SUCCESS_GREEN + "; "
    "   border-radius: 12px; "
    "   margin-top: 16px; "
    "   padding: 20px; "
    "   font-weight: 600; "
    "   background: " + CARD_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QGroupBox.success::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 8px 16px; "
    "   background: " + SUCCESS_GREEN + "; "
    "   color: white; "
    "   border-radius: 8px; "
    "   left: 16px; "
    "   font-size: 14px; "
    "   font-weight: 600; "
    "}";

// Primary Card
const QString GROUP_BOX_FIRE = 
    "QGroupBox.fire { "
    "   border: 1px solid " + ADOBE_BLUE + "; "
    "   border-radius: 12px; "
    "   margin-top: 16px; "
    "   padding: 20px; "
    "   font-weight: 600; "
    "   background: " + CARD_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QGroupBox.fire::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 8px 16px; "
    "   background: " + ADOBE_BLUE + "; "
    "   color: white; "
    "   border-radius: 8px; "
    "   left: 16px; "
    "   font-size: 14px; "
    "   font-weight: 600; "
    "}";

// Warning Card
const QString GROUP_BOX_URGENT = 
    "QGroupBox.urgent { "
    "   border: 1px solid " + WARNING_ORANGE + "; "
    "   border-radius: 12px; "
    "   margin-top: 16px; "
    "   padding: 20px; "
    "   font-weight: 600; "
    "   background: " + CARD_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QGroupBox.urgent::title { "
    "   subcontrol-origin: margin; "
    "   subcontrol-position: top left; "
    "   padding: 8px 16px; "
    "   background: " + WARNING_ORANGE + "; "
    "   color: white; "
    "   border-radius: 8px; "
    "   left: 16px; "
    "   font-size: 14px; "
    "   font-weight: 600; "
    "}";

// Modern Tab Widget
const QString TAB_WIDGET = 
    "QTabWidget::pane { "
    "   border: 1px solid " + BORDER_GRAY + "; "
    "   border-radius: 8px; "
    "   background: " + CARD_BG + "; "
    "   top: -1px; "
    "} "
    "QTabBar::tab { "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_SECONDARY + "; "
    "   padding: 12px 20px; "
    "   margin-right: 2px; "
    "   border-top-left-radius: 8px; "
    "   border-top-right-radius: 8px; "
    "   font-size: 14px; "
    "   font-weight: 500; "
    "   min-width: 120px; "
    "   border: 1px solid transparent; "
    "   border-bottom: none; "
    "} "
    "QTabBar::tab:hover { "
    "   background: " + HOVER_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "} "
    "QTabBar::tab:selected { "
    "   background: " + CARD_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   border-color: " + BORDER_GRAY + "; "
    "   font-weight: 600; "
    "} "
    "QTabBar::tab:!selected { "
    "   margin-top: 2px; "
    "}";

// ==================== STATUS INDICATORS ====================

// Status Indicator Styles
const QString STATUS_SUCCESS = 
    "QLabel.status-success { "
    "   color: " + SUCCESS_GREEN + "; "
    "   font-weight: 600; "
    "   background: rgba(34, 139, 34, 0.1); "
    "   border: 1px solid " + SUCCESS_GREEN + "; "
    "   border-radius: 6px; "
    "   padding: 8px 12px; "
    "}";

const QString STATUS_WARNING = 
    "QLabel.status-warning { "
    "   color: " + WARNING_ORANGE + "; "
    "   font-weight: 600; "
    "   background: rgba(255, 140, 0, 0.1); "
    "   border: 1px solid " + WARNING_ORANGE + "; "
    "   border-radius: 6px; "
    "   padding: 8px 12px; "
    "}";

const QString STATUS_ERROR = 
    "QLabel.status-error { "
    "   color: " + ERROR_RED + "; "
    "   font-weight: 600; "
    "   background: rgba(220, 20, 60, 0.1); "
    "   border: 1px solid " + ERROR_RED + "; "
    "   border-radius: 6px; "
    "   padding: 8px 12px; "
    "}";

const QString STATUS_INFO = 
    "QLabel.status-info { "
    "   color: " + INFO_CYAN + "; "
    "   font-weight: 600; "
    "   background: rgba(32, 178, 170, 0.1); "
    "   border: 1px solid " + INFO_CYAN + "; "
    "   border-radius: 6px; "
    "   padding: 8px 12px; "
    "}";

// Progress Bar
const QString PROGRESS_BAR = 
    "QProgressBar { "
    "   border: 2px solid " + BORDER_GRAY + "; "
    "   border-radius: 8px; "
    "   background: " + SURFACE_BG + "; "
    "   text-align: center; "
    "   color: " + TEXT_PRIMARY + "; "
    "   font-weight: 600; "
    "   height: 20px; "
    "} "
    "QProgressBar::chunk { "
    "   background: " + GRADIENT_PRIMARY + "; "
    "   border-radius: 6px; "
    "}";

// ==================== ENHANCED FORM STYLING ====================

// Modern Input Fields (Enhanced)
const QString LINE_EDIT = 
    "QLineEdit { "
    "   border: 2px solid " + BORDER_GRAY + "; "
    "   border-radius: 10px; "
    "   padding: 14px 18px; "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   font-size: 14px; "
    "   min-height: 24px; "
    "   transition: all 0.2s ease; "
    "} "
    "QLineEdit:focus { "
    "   border-color: " + NMERA_PRIMARY + "; "
    "   border-width: 2px; "
    "   background: " + CARD_BG + "; "
    "   box-shadow: 0 0 0 3px rgba(210, 105, 30, 0.1); "
    "} "
    "QLineEdit:disabled { "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_MUTED + "; "
    "   border-color: " + BORDER_GRAY + "; "
    "} "
    "QTextEdit { "
    "   border: 2px solid " + BORDER_GRAY + "; "
    "   border-radius: 10px; "
    "   padding: 14px 18px; "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   font-size: 14px; "
    "   min-height: 80px; "
    "   transition: all 0.2s ease; "
    "} "
    "QTextEdit:focus { "
    "   border-color: " + NMERA_PRIMARY + "; "
    "   border-width: 2px; "
    "   background: " + CARD_BG + "; "
    "   box-shadow: 0 0 0 3px rgba(210, 105, 30, 0.1); "
    "} "
    "QTextEdit:disabled { "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_MUTED + "; "
    "}";

// Modern ComboBox
const QString COMBO_BOX = 
    "QComboBox { "
    "   border: 1px solid " + BORDER_GRAY + "; "
    "   border-radius: 8px; "
    "   padding: 12px 16px; "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   font-size: 14px; "
    "   min-width: 120px; "
    "   min-height: 20px; "
    "} "
    "QComboBox:focus { "
    "   border-color: " + ADOBE_BLUE + "; "
    "   border-width: 2px; "
    "   background: " + CARD_BG + "; "
    "} "
    "QComboBox::drop-down { "
    "   border: none; "
    "   background: " + BORDER_GRAY + "; "
    "   width: 32px; "
    "   border-top-right-radius: 7px; "
    "   border-bottom-right-radius: 7px; "
    "} "
    "QComboBox::down-arrow { "
    "   image: none; "
    "   border-left: 5px solid transparent; "
    "   border-right: 5px solid transparent; "
    "   border-top: 6px solid " + TEXT_SECONDARY + "; "
    "   margin-right: 8px; "
    "} "
    "QComboBox QAbstractItemView { "
    "   border: 1px solid " + BORDER_GRAY + "; "
    "   border-radius: 8px; "
    "   selection-background-color: " + SELECTED_BG + "; "
    "   selection-color: white; "
    "   background: " + CARD_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   font-size: 14px; "
    "   padding: 8px; "
    "} "
    "QSpinBox, QDoubleSpinBox, QDateEdit { "
    "   border: 1px solid " + BORDER_GRAY + "; "
    "   border-radius: 8px; "
    "   padding: 12px 16px; "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   font-size: 14px; "
    "   min-height: 20px; "
    "} "
    "QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus { "
    "   border-color: " + ADOBE_BLUE + "; "
    "   border-width: 2px; "
    "   background: " + CARD_BG + "; "
    "}";

// Modern Label Styles
const QString LABEL_HEADER = 
    "QLabel.header { "
    "   font-size: 24px; "
    "   font-weight: 700; "
    "   color: " + TEXT_PRIMARY + "; "
    "   padding: 16px 0px; "
    "}";

const QString LABEL_SUBHEADER = 
    "QLabel.subheader { "
    "   font-size: 18px; "
    "   font-weight: 600; "
    "   color: " + TEXT_PRIMARY + "; "
    "   padding: 12px 0px; "
    "}";

// Form labels
const QString FORM_LABELS = 
    "QLabel { "
    "   color: " + TEXT_SECONDARY + "; "
    "   font-size: 14px; "
    "   font-weight: 500; "
    "   padding: 4px 0px; "
    "}";

// Modern Text Edit
const QString TEXT_EDIT = 
    "QTextEdit { "
    "   border: 1px solid " + BORDER_GRAY + "; "
    "   border-radius: 8px; "
    "   padding: 12px 16px; "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_PRIMARY + "; "
    "   font-size: 14px; "
    "   min-height: 80px; "
    "} "
    "QTextEdit:focus { "
    "   border-color: " + ADOBE_BLUE + "; "
    "   border-width: 2px; "
    "   background: " + CARD_BG + "; "
    "} "
    "QTextEdit:disabled { "
    "   background: " + SURFACE_BG + "; "
    "   color: " + TEXT_MUTED + "; "
    "}";

// Modern Header
const QString ORG_HEADER = 
    "QLabel.orgHeader { "
    "   background: " + GRADIENT_PRIMARY + "; "
    "   color: white; "
    "   border-radius: 12px; "
    "   padding: 20px 24px; "
    "   font-size: 20px; "
    "   font-weight: 700; "
    "}";

// ==================== COMBINED STYLESHEET ====================

// Complete modern application stylesheet with NM-ERA theming
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
    ORG_HEADER +
    STATUS_SUCCESS +
    STATUS_WARNING +
    STATUS_ERROR +
    STATUS_INFO +
    PROGRESS_BAR;

} // namespace AdobeStyles

