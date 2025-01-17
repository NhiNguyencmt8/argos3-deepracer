#include "deepracer_entity.h"

#include <argos3/core/simulator/entity/controllable_entity.h>
#include <argos3/core/simulator/entity/embodied_entity.h>
#include <argos3/core/simulator/space/space.h>
#include <argos3/plugins/simulator/entities/battery_equipped_entity.h>
#include <argos3/plugins/simulator/entities/proximity_sensor_equipped_entity.h>
#include <argos3/plugins/simulator/entities/rab_equipped_entity.h>

#include "deepracer_measures.h"

namespace argos {

    /****************************************/
    /****************************************/

    CDeepracerEntity::CDeepracerEntity()
        : CComposableEntity(NULL),
          m_pcControllableEntity(NULL),
          m_pcEmbodiedEntity(NULL),
          m_pcLIDARSensorEquippedEntity(NULL),
          m_pcRABEquippedEntity(NULL),
          m_pcAckermannWheeledEntity(NULL) {
    }

    /****************************************/
    /****************************************/

    CDeepracerEntity::CDeepracerEntity(const std::string& str_id,
                                       const std::string& str_controller_id,
                                       const CVector3&    c_position,
                                       const CQuaternion& c_orientation,
                                       Real               f_rab_range,
                                       size_t             un_rab_data_size,
                                       const std::string& str_bat_model)
        : CComposableEntity(NULL, str_id),
          m_pcControllableEntity(NULL),
          m_pcEmbodiedEntity(NULL),
          m_pcLIDARSensorEquippedEntity(NULL),
          m_pcRABEquippedEntity(NULL),
          m_pcAckermannWheeledEntity(NULL) {
        try {
            /*
             * Create and init components
             */
            /* Embodied entity */
            m_pcEmbodiedEntity = new CEmbodiedEntity(this, "body_0", c_position, c_orientation);
            AddComponent(*m_pcEmbodiedEntity);
            /* Wheeled entity and wheel positions (rear left, rear right, front left, front right) */
            m_pcAckermannWheeledEntity = new CAckermannWheeledEntity(this, "wheels_0");
            AddComponent(*m_pcAckermannWheeledEntity);
            m_pcAckermannWheeledEntity->SetWheel(0,
                                                 DEEPRACER_REAR_LEFT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            m_pcAckermannWheeledEntity->SetWheel(1,
                                                 DEEPRACER_REAR_RIGHT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            m_pcAckermannWheeledEntity->SetWheel(2,
                                                 DEEPRACER_FRONT_LEFT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            m_pcAckermannWheeledEntity->SetWheel(3,
                                                 DEEPRACER_FRONT_RIGHT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            /* Camera sensor equipped entity */
            /* LIDAR sensor equipped entity */
            m_pcLIDARSensorEquippedEntity =
                new CProximitySensorEquippedEntity(this,
                                                   "lidar");
            AddComponent(*m_pcLIDARSensorEquippedEntity);
            /* RAB equipped entity */
            m_pcRABEquippedEntity =
                new CRABEquippedEntity(this,
                                       "rab_0",
                                       un_rab_data_size,
                                       f_rab_range,
                                       m_pcEmbodiedEntity->GetOriginAnchor(),
                                       *m_pcEmbodiedEntity,
                                       CVector3(0.0f, 0.0f, DEEPRACER_BASE_TOP));
            AddComponent(*m_pcRABEquippedEntity);
            /* Battery equipped entity */
            m_pcBatteryEquippedEntity = new CBatteryEquippedEntity(this, "battery_0", str_bat_model);
            AddComponent(*m_pcBatteryEquippedEntity);
            /* Controllable entity
                It must be the last one, for actuators/sensors to link to composing entities correctly */
            m_pcControllableEntity = new CControllableEntity(this, "controller_0");
            AddComponent(*m_pcControllableEntity);
            m_pcControllableEntity->SetController(str_controller_id);
            /* Update components */
            UpdateComponents();
        } catch (CARGoSException& ex) {
            THROW_ARGOSEXCEPTION_NESTED("Failed to initialize entity \"" << GetId() << "\".", ex);
        }
    }

    /****************************************/
    /****************************************/

    void CDeepracerEntity::Init(TConfigurationNode& t_tree) {
        try {
            /*
             * Init parent
             */
            CComposableEntity::Init(t_tree);
            /*
             * Create and init components
             */
            /* Embodied entity */
            m_pcEmbodiedEntity = new CEmbodiedEntity(this);
            AddComponent(*m_pcEmbodiedEntity);
            m_pcEmbodiedEntity->Init(GetNode(t_tree, "body"));
            /* Wheeled entity and wheel positions (rear left, rear right, front left, front right) */
            m_pcAckermannWheeledEntity = new CAckermannWheeledEntity(this, "wheels_0");
            AddComponent(*m_pcAckermannWheeledEntity);
            m_pcAckermannWheeledEntity->SetWheel(0,
                                                 DEEPRACER_REAR_LEFT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            m_pcAckermannWheeledEntity->SetWheel(1,
                                                 DEEPRACER_REAR_RIGHT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            m_pcAckermannWheeledEntity->SetWheel(2,
                                                 DEEPRACER_FRONT_LEFT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            m_pcAckermannWheeledEntity->SetWheel(3,
                                                 DEEPRACER_FRONT_RIGHT_WHEEL_POS_WRT_BASE,
                                                 DEEPRACER_WHEEL_RADIUS);
            /* Camera sensor equipped entity */
            // TODO: populate this
            /* LIDAR sensor equipped entity */
            m_pcLIDARSensorEquippedEntity =
                new CProximitySensorEquippedEntity(this,
                                                   "lidar");
            AddComponent(*m_pcLIDARSensorEquippedEntity);
            /* RAB equipped entity */
            Real fRange = 3.0f;
            GetNodeAttributeOrDefault(t_tree, "rab_range", fRange, fRange);
            UInt32 unDataSize = 10;
            GetNodeAttributeOrDefault(t_tree, "rab_data_size", unDataSize, unDataSize);
            m_pcRABEquippedEntity =
                new CRABEquippedEntity(this,
                                       "rab_0",
                                       unDataSize,
                                       fRange,
                                       m_pcEmbodiedEntity->GetOriginAnchor(),
                                       *m_pcEmbodiedEntity,
                                       CVector3(0.0f, 0.0f, DEEPRACER_BASE_TOP));
            AddComponent(*m_pcRABEquippedEntity);
            /* Battery equipped entity */
            m_pcBatteryEquippedEntity = new CBatteryEquippedEntity(this, "battery_0");
            if (NodeExists(t_tree, "battery"))
                m_pcBatteryEquippedEntity->Init(GetNode(t_tree, "battery"));
            AddComponent(*m_pcBatteryEquippedEntity);
            /* Controllable entity
               It must be the last one, for actuators/sensors to link to composing entities correctly */
            m_pcControllableEntity = new CControllableEntity(this);
            AddComponent(*m_pcControllableEntity);
            m_pcControllableEntity->Init(GetNode(t_tree, "controller"));
            /* Update components */
            UpdateComponents();
        } catch (CARGoSException& ex) {
            THROW_ARGOSEXCEPTION_NESTED("Failed to initialize entity \"" << GetId() << "\".", ex);
        }
    }

    /****************************************/
    /****************************************/

    void CDeepracerEntity::Reset() {
        /* Reset all components */
        CComposableEntity::Reset();
        /* Update components */
        UpdateComponents();
    }

    /****************************************/
    /****************************************/

    void CDeepracerEntity::Destroy() {
        CComposableEntity::Destroy();
    }

    /****************************************/
    /****************************************/

    void CDeepracerEntity::UpdateComponents() {
        // if (m_pcCameraSensorEquippedEntity->IsEnabled())
        //     m_pcCameraSensorEquippedEntity->Update();
        if (m_pcLIDARSensorEquippedEntity->IsEnabled())
            m_pcLIDARSensorEquippedEntity->Update();
        if (m_pcRABEquippedEntity->IsEnabled())
            m_pcRABEquippedEntity->Update();
        if (m_pcBatteryEquippedEntity->IsEnabled())
            m_pcBatteryEquippedEntity->Update();
    }

    /****************************************/
    /****************************************/

    REGISTER_ENTITY(CDeepracerEntity,
                    "deepracer",
                    "Carlo Pinciroli [ilpincy@gmail.com], Khai Yi Chin [khaiyichin@gmail.com]",
                    "1.0",
                    "The AWS DeepRacer robot.",
                    "The AWS DeepRacer is a commercial, extensible robot produced by Amazon. More\n"
                    "information is available at https://aws.amazon.com/deepracer\n\n"
                    "REQUIRED XML CONFIGURATION\n\n"
                    "  <arena ...>\n"
                    "    ...\n"
                    "    <deepracer id=\"eb0\">\n"
                    "      <body position=\"0.4,2.3,0.25\" orientation=\"45,90,0\" />\n"
                    "      <controller config=\"mycntrl\" />\n"
                    "    </deepracer>\n"
                    "    ...\n"
                    "  </arena>\n\n"
                    "The 'id' attribute is necessary and must be unique among the entities. If two\n"
                    "entities share the same id, initialization aborts.\n"
                    "The 'body/position' attribute specifies the position of the bottom point of the\n"
                    "AWS DeepRacer in the arena. When the robot is untranslated and unrotated, the\n"
                    "bottom point is in the origin and it is defined as the middle point between\n"
                    "the two wheels on the XY plane and the lowest point of the robot on the Z\n"
                    "axis, that is the point where the wheels touch the floor. The attribute values\n"
                    "are in the X,Y,Z order.\n"
                    "The 'body/orientation' attribute specifies the orientation of the AWS DeepRacer.\n"
                    "All rotations are performed with respect to the bottom point. The order of the\n"
                    "angles is Z,Y,X, which means that the first number corresponds to the rotation\n"
                    "around the Z axis, the second around Y and the last around X. This reflects\n"
                    "the internal convention used in ARGoS, in which rotations are performed in\n"
                    "that order. Angles are expressed in degrees. When the robot is unrotated, it\n"
                    "is oriented along the X axis.\n"
                    "The 'controller/config' attribute is used to assign a controller to the\n"
                    "AWS DeepRacer. The value of the attribute must be set to the id of a previously\n"
                    "defined controller. Controllers are defined in the <controllers> XML subtree.\n\n"
                    "OPTIONAL XML CONFIGURATION\n\n"
                    "You can set the emission range of the range-and-bearing system. By default, a\n"
                    "message sent by a AWS DeepRacer can be received up to 3m. By using the 'rab_range'\n"
                    "attribute, you can change it to, i.e., 4m as follows:\n\n"
                    "  <arena ...>\n"
                    "    ...\n"
                    "    <deepracer id=\"dr0\" rab_range=\"4\">\n"
                    "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                    "      <controller config=\"mycntrl\" />\n"
                    "    </deepracer>\n"
                    "    ...\n"
                    "  </arena>\n\n"
                    "You can also set the data sent at each time step through the range-and-bearing\n"
                    "system. By default, a message sent by an AWS DeepRacer is 50 bytes long. By using the\n"
                    "'rab_data_size' attribute, you can change it to, i.e., 100 bytes as follows:\n\n"
                    "  <arena ...>\n"
                    "    ...\n"
                    "    <deepracer id=\"dr0\" rab_data_size=\"100\">\n"
                    "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                    "      <controller config=\"mycntrl\" />\n"
                    "    </deepracer>\n"
                    "    ...\n"
                    "  </arena>\n\n"
                    "You can also configure the battery of the robot. By default, the battery never\n"
                    "depletes. You can choose among several battery discharge models, such as\n"
                    "- time: the battery depletes by a fixed amount at each time step\n"
                    "- motion: the battery depletes according to how the robot moves\n"
                    "- time_motion: a combination of the above models.\n"
                    "You can define your own models too. Follow the examples in the file\n"
                    "argos3/src/plugins/simulator/entities/battery_equipped_entity.cpp.\n\n"
                    "  <arena ...>\n"
                    "    ...\n"
                    "    <deepracer id=\"dr0\"\n"
                    "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                    "      <controller config=\"mycntrl\" />\n"
                    "      <battery model=\"time\" factor=\"1e-5\"/>\n"
                    "    </deepracer>\n"
                    "    ...\n"
                    "  </arena>\n\n"
                    "  <arena ...>\n"
                    "    ...\n"
                    "    <deepracer id=\"dr0\"\n"
                    "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                    "      <controller config=\"mycntrl\" />\n"
                    "      <battery model=\"motion\" pos_factor=\"1e-3\"\n"
                    "                              orient_factor=\"1e-3\"/>\n"
                    "    </deepracer>\n"
                    "    ...\n"
                    "  </arena>\n\n"
                    "  <arena ...>\n"
                    "    ...\n"
                    "    <deepracer id=\"dr0\"\n"
                    "      <body position=\"0.4,2.3,0.25\" orientation=\"45,0,0\" />\n"
                    "      <controller config=\"mycntrl\" />\n"
                    "      <battery model=\"time_motion\" time_factor=\"1e-5\"\n"
                    "                                   pos_factor=\"1e-3\"\n"
                    "                                   orient_factor=\"1e-3\"/>\n"
                    "    </deepracer>\n"
                    "    ...\n"
                    "  </arena>\n\n",
                    "Under development");

    /****************************************/
    /****************************************/

    REGISTER_STANDARD_SPACE_OPERATIONS_ON_COMPOSABLE(CDeepracerEntity);

    /****************************************/
    /****************************************/

}