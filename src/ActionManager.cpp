//
// Created by liugantang on 2025/9/29.
//

#include "ActionManager.h"
#include <QKeySequence>

/**
 * @brief Returns the singleton instance of the ActionManager.
 *
 * This static method ensures that there is only one instance of the ActionManager
 * throughout the application's lifecycle.
 */
ActionManager &ActionManager::instance() {
    static ActionManager instance;
    return instance;
}

/**
 * @brief Retrieves a QAction pointer from the map using its ActionID.
 *
 * @param id The enum ID of the action to retrieve.
 * @return A pointer to the QAction, or nullptr if not found.
 */
QAction *ActionManager::action(ActionID id) {
    return m_actions.value(id, nullptr);
}

/**
 * @brief Constructor for the ActionManager.
 *
 * It initializes the manager by creating all actions and binding their signals.
 * It's private to enforce the singleton pattern.
 *
 * @param parent The parent QObject, typically null for a global singleton.
 */
ActionManager::ActionManager(QObject *parent) : QObject(parent) {
    createActions();
    bindSignals();
}

/**
 * @brief Destructor.
 *
 * The QObject parent-child relationship automatically handles the deletion
 * of all QAction objects created with `this` as their parent.
 */
ActionManager::~ActionManager() = default;

/**
 * @brief Creates and configures all application actions.
 *
 * This method instantiates each QAction, sets its user-visible text (using tr for i18n),
 * assigns a standard keyboard shortcut, and stores it in the actions map.
 */
void ActionManager::createActions() {
    // --- File Actions ---
    m_actions[ActionOpenFile] = new QAction(tr("&Open File..."), this);
    m_actions[ActionOpenFile]->setShortcut(QKeySequence::Open); // Ctrl+O

    m_actions[ActionOpenFolder] = new QAction(tr("Open &Folder..."), this);
    m_actions[ActionOpenFolder]->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));

    m_actions[ActionClose] = new QAction(tr("&Close Image"), this);
    m_actions[ActionClose]->setShortcut(QKeySequence::Close); // Ctrl+W

    m_actions[ActionSelectImage] = new QAction(tr("&Select Image..."), this);

    m_actions[ActionShowInExplorer] = new QAction(tr("Show in &Explorer"), this);

    m_actions[ActionExit] = new QAction(tr("E&xit"), this);
    m_actions[ActionExit]->setShortcut(QKeySequence::Quit); // Ctrl+Q

    // --- Tools/Help Actions ---
    m_actions[ActionConfiguration] = new QAction(tr("&Settings..."), this);
    m_actions[ActionAbout] = new QAction(tr("&About..."), this);
    m_actions[ActionAbout]->setShortcut(QKeySequence::HelpContents); // F1

    // --- Filter Actions ---
    m_actions[ActionFilterNone] = new QAction(tr("&No Filter"), this);
    m_actions[ActionFilterSoften] = new QAction(tr("&Soften"), this);
    m_actions[ActionFilterSharpen] = new QAction(tr("S&harpen"), this);

    // --- Navigation Actions ---
    m_actions[ActionNextImage] = new QAction(tr("Next Image"), this);
    m_actions[ActionNextImage]->setShortcut(QKeySequence(Qt::Key_Right));

    m_actions[ActionPreviousImage] = new QAction(tr("Previous Image"), this);
    m_actions[ActionPreviousImage]->setShortcut(QKeySequence(Qt::Key_Left));

    m_actions[ActionFirstImage] = new QAction(tr("First Image"), this);
    m_actions[ActionFirstImage]->setShortcut(QKeySequence(Qt::Key_Home));

    m_actions[ActionLastImage] = new QAction(tr("Last Image"), this);
    m_actions[ActionLastImage]->setShortcut(QKeySequence(Qt::Key_End));

    m_actions[ActionNext10Image] = new QAction(tr("Forward 10 Images"), this);
    m_actions[ActionNext10Image]->setShortcut(QKeySequence(Qt::Key_PageDown));

    m_actions[ActionPrevious10Image] = new QAction(tr("Back 10 Images"), this);
    m_actions[ActionPrevious10Image]->setShortcut(QKeySequence(Qt::Key_PageUp));

    m_actions[ActionNextRandImage] = new QAction(tr("Next Random Image"), this);
    m_actions[ActionPreviousRandImage] = new QAction(tr("Previous Random Image"), this);

    m_actions[ActionNextFolderOrCompression] = new QAction(tr("Next Folder/Archive"), this);
    m_actions[ActionPreviousFolderOrCompression] = new QAction(tr("Previous Folder/Archive"), this);
}

/**
 * @brief Connects all action signals to the manager's signals.
 *
 * This method iterates through all created actions and connects their `triggered()`
 * signal to a lambda function that, in turn, emits the appropriate signal from
 * the ActionManager itself. This decouples the UI from the application logic.
 */
void ActionManager::bindSignals() {
    // File
    connect(action(ActionOpenFile), &QAction::triggered, this, [this] { emit requestOpenFile(); });
    connect(action(ActionOpenFolder), &QAction::triggered, this, [this] { emit requestOpenFolder(); });
    connect(action(ActionClose), &QAction::triggered, this, [this] { emit requestClose(); });
    connect(action(ActionSelectImage), &QAction::triggered, this, [this] { emit requestSelectImage(); });
    connect(action(ActionShowInExplorer), &QAction::triggered, this, [this] { emit requestFilePosition(); });
    connect(action(ActionExit), &QAction::triggered, this, [this] { emit requestExit(); });

    connect(action(ActionConfiguration), &QAction::triggered, this, [this] { emit requestConfiguration(); });
    connect(action(ActionAbout), &QAction::triggered, this, [this] { emit requestAbout(); });

    // Filter
    connect(action(ActionFilterNone), &QAction::triggered, this, [this] { emit requestNoFilter(); });
    connect(action(ActionFilterSoften), &QAction::triggered, this, [this] { emit requestFilterSoften(); });
    connect(action(ActionFilterSharpen), &QAction::triggered, this, [this] { emit requestFilterSharpen(); });

    // Navigation
    connect(action(ActionNextImage), &QAction::triggered, this, [this] { emit requestNextImage(); });
    connect(action(ActionPreviousImage), &QAction::triggered, this, [this] { emit requestPreviousImage(); });
    connect(action(ActionFirstImage), &QAction::triggered, this, &ActionManager::requestFirstImage);
    connect(action(ActionLastImage), &QAction::triggered, this, &ActionManager::requestLastImage);
    connect(action(ActionNext10Image), &QAction::triggered, this, [this] { emit requestNext10Image(); });
    connect(action(ActionPrevious10Image), &QAction::triggered, this, [this] { emit requestPrevious10Image(); });
    connect(action(ActionNextRandImage), &QAction::triggered, this, [this] { emit requestNextRandImage(); });
    connect(action(ActionPreviousRandImage), &QAction::triggered, this, [this] { emit requestPreviousRandImage(); });
    connect(action(ActionNextFolderOrCompression), &QAction::triggered, this,
            [this] { emit requestNextFolderOrCompression(); });
    connect(action(ActionPreviousFolderOrCompression), &QAction::triggered, this,
            [this] { emit requestPreviousFolderOrCompression(); });
}
