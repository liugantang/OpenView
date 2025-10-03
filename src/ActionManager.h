//
// Created by liugantang on 2025/9/29.
//

#ifndef OPENVIEW_COMMONACTION_H
#define OPENVIEW_COMMONACTION_H

#include <QAction>
#include <QObject>
#include <QMap>

/**
 * @enum ActionID
 * @brief Defines unique identifiers for all actions used in the application.
 */
enum ActionID {
    // File Menu Actions
    ActionOpenFile,
    ActionOpenFolder,
    ActionClose,
    ActionSelectImage,
    ActionShowInExplorer,
    ActionExit,
    ActionConfiguration,
    ActionAbout,

    // Filter Menu Actions
    ActionFilterNone,
    ActionFilterSoften,
    ActionFilterSharpen,

    // Navigation Actions
    ActionNextImage,
    ActionPreviousImage,
    ActionFirstImage,
    ActionLastImage,
    ActionNext10Image,
    ActionPrevious10Image,
    ActionNextRandImage,
    ActionPreviousRandImage,
    ActionNextFolderOrCompression,
    ActionPreviousFolderOrCompression,
};

/**
 * @class ActionManager
 * @brief A singleton class to manage all QAction objects in the application.
 *
 * This class centralizes the creation, storage, and signal handling for all actions.
 * It ensures that actions are created only once and can be accessed from anywhere
 * in the application via the instance() method. When an action is triggered, this
 * manager emits a corresponding signal.
 */
class ActionManager : public QObject {
Q_OBJECT

public:
    /**
     * @brief Provides access to the singleton instance of the ActionManager.
     * @return A reference to the single ActionManager object.
     */
    static ActionManager &instance();

    /**
     * @brief Retrieves a specific action by its ID.
     * @param id The ActionID of the desired action.
     * @return A pointer to the QAction, or nullptr if the ID is not found.
     */
    QAction *action(ActionID id);

signals:
    // Signals emitted when the corresponding action is triggered
    void requestOpenFile();
    void requestOpenFolder();
    void requestClose();
    void requestSelectImage();
    void requestFilePosition();
    void requestExit();
    void requestConfiguration();
    void requestAbout();
    void requestNoFilter();
    void requestFilterSoften();
    void requestFilterSharpen();
    void requestNextImage();
    void requestPreviousImage();
    void requestFirstImage();
    void requestLastImage();
    void requestNext10Image();
    void requestPrevious10Image();
    void requestNextRandImage();
    void requestPreviousRandImage();
    void requestNextFolderOrCompression();
    void requestPreviousFolderOrCompression();

private:
    /**
     * @brief Private constructor to enforce the singleton pattern.
     * @param parent The parent QObject.
     */
    ActionManager(QObject *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~ActionManager();

    // Prevent copying and assignment
    ActionManager(const ActionManager&) = delete;
    ActionManager& operator=(const ActionManager&) = delete;

    /**
     * @brief Creates all QAction objects and stores them in the m_actions map.
     */
    void createActions();

    /**
     * @brief Connects the triggered() signal of each QAction to a lambda
     * that emits the corresponding request signal from this manager.
     */
    void bindSignals();

    ///< A map to store all actions, keyed by their ActionID.
    QMap<ActionID, QAction *> m_actions;
};


#endif //OPENVIEW_COMMONACTION_H