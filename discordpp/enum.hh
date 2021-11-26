//
// Created by Aidan on 8/29/2021.
//

#pragma once

namespace discordpp {

enum class ActivityFlags : int {
    INSTANCE = 1 << 0,
    JOIN = 1 << 1,
    SPECTATE = 1 << 2,
    JOIN_REQUEST = 1 << 3,
    SYNC = 1 << 4,
    PLAY = 1 << 5
};

enum class AllowedMentionType { roles, users, everyone };
NLOHMANN_JSON_SERIALIZE_ENUM(AllowedMentionType,
                             {{AllowedMentionType::roles, "roles"},
                              {AllowedMentionType::users, "users"},
                              {AllowedMentionType::everyone, "everyone"}})

enum class ApplicationCommandType : int {
    CHAT_INPUT = 1,
    USER = 2,
    MESSAGE = 3
};

enum class ApplicationCommandOptionType : int {
    SUB_COMMAND = 1,
    SUB_COMMAND_GROUP = 2,
    STRING = 3,
    INTEGER = 4,
    BOOLEAN = 5,
    USER = 6,
    CHANNEL = 7,
    ROLE = 8,
    MENTIONABLE = 9,
    NUMBER = 10
};

enum class ApplicationCommandPermissionType : int { ROLE = 1, USER = 2 };

enum class ApplicationFlags : int {
    GATEWAY_PRESENCE = 1 << 12,
    GATEWAY_PRESENCE_LIMITED = 1 << 13,
    GATEWAY_GUILD_MEMBERS = 1 << 14,
    GATEWAY_GUILD_MEMBERS_LIMITED = 1 << 15,
    VERIFICATION_PENDING_GUILD_LIMIT = 1 << 16,
    EMBEDDED = 1 << 17
};

enum class AuditLogEvent : int {
    GUILD_UPDATE = 1,
    CHANNEL_CREATE = 10,
    CHANNEL_UPDATE = 11,
    CHANNEL_DELETE = 12,
    CHANNEL_OVERWRITE_CREATE = 13,
    CHANNEL_OVERWRITE_UPDATE = 14,
    CHANNEL_OVERWRITE_DELETE = 15,
    MEMBER_KICK = 20,
    MEMBER_PRUNE = 21,
    MEMBER_BAN_ADD = 22,
    MEMBER_BAN_REMOVE = 23,
    MEMBER_UPDATE = 24,
    MEMBER_ROLE_UPDATE = 25,
    MEMBER_MOVE = 26,
    MEMBER_DISCONNECT = 27,
    BOT_ADD = 28,
    ROLE_CREATE = 30,
    ROLE_UPDATE = 31,
    ROLE_DELETE = 32,
    INVITE_CREATE = 40,
    INVITE_UPDATE = 41,
    INVITE_DELETE = 42,
    WEBHOOK_CREATE = 50,
    WEBHOOK_UPDATE = 51,
    WEBHOOK_DELETE = 52,
    EMOJI_CREATE = 60,
    EMOJI_UPDATE = 61,
    EMOJI_DELETE = 62,
    MESSAGE_DELETE = 72,
    MESSAGE_BULK_DELETE = 73,
    MESSAGE_PIN = 74,
    MESSAGE_UNPIN = 75,
    INTEGRATION_CREATE = 80,
    INTEGRATION_UPDATE = 81,
    INTEGRATION_DELETE = 82,
    STAGE_INSTANCE_CREATE = 83,
    STAGE_INSTANCE_UPDATE = 84,
    STAGE_INSTANCE_DELETE = 85,
    STICKER_CREATE = 90,
    STICKER_UPDATE = 91,
    STICKER_DELETE = 92,
    THREAD_CREATE = 110,
    THREAD_UPDATE = 111,
    THREAD_DELETE = 112,
};

enum ButtonStyle : int {
    PRIMARY = 1,
    SECONDARY = 2,
    SUCCESS = 3,
    DANGER = 4,
    LINK = 5
};

// https://discord.com/developers/docs/resources/channel#channel-object-channel-types
enum class ChannelType : int {
    GUILD_TEXT = 0,
    DM = 1,
    GUILD_VOICE = 2,
    GROUP_DM = 3,
    GUILD_CATEGORY = 4,
    GUILD_NEWS = 5,
    GUILD_STORE = 6,
    GUILD_NEWS_THREAD = 10,
    GUILD_PUBLIC_THREAD = 11,
    GUILD_PRIVATE_THREAD = 12,
    GUILD_STAGE_VOICE = 13
};

enum class ComponentType : int { ACTIONROW = 1, BUTTON = 2, SELECTMENU = 3 };

enum class DefaultMessageNotificationLevel : int {
    ALL_MESSAGES = 0,
    ONLY_MENTIONS = 1
};

enum class DeviceType { AUDIO_INPUT, AUDIO_OUTPUT, VIDEO_INPUT };
NLOHMANN_JSON_SERIALIZE_ENUM(DeviceType,
                             {{DeviceType::AUDIO_INPUT, "audioinput"},
                              {DeviceType::AUDIO_OUTPUT, "audiooutput"},
                              {DeviceType::VIDEO_INPUT, "videoinput"}})

enum class EmbedType { rich, image, video, gifv, article, link };
NLOHMANN_JSON_SERIALIZE_ENUM(EmbedType, {{EmbedType::rich, "rich"},
                                         {EmbedType::image, "image"},
                                         {EmbedType::video, "video"},
                                         {EmbedType::gifv, "gifv"},
                                         {EmbedType::article, "article"},
                                         {EmbedType::link, "link"}})

enum class ExplicitContentFilterLevel : int {
    DISABLED = 0,
    MEMBERS_WITHOUT_ROLES = 1,
    ALL_MEMBERS = 2
};

enum class GuildFeature {
    ANIMATED_ICON,
    BANNER,
    COMMERCE,
    COMMUNITY,
    DISCOVERABLE,
    FEATURABLE,
    INVITE_SPLASH,
    MEMBER_VERIFICATION_GATE_ENABLED,
    MONETIZATION_ENABLED,
    MORE_STICKERS,
    NEWS,
    PARTNERED,
    PREVIEW_ENABLED,
    PRIVATE_THREADS,
    ROLE_ICONS,
    SEVEN_DAY_THREAD_ARCHIVE,
    THREE_DAY_THREAD_ARCHIVE,
    TICKETED_EVENTS_ENABLED,
    VANITY_URL,
    VERIFIED,
    VIP_REGIONS,
    WELCOME_SCREEN_ENABLED
};
NLOHMANN_JSON_SERIALIZE_ENUM(
    GuildFeature,
    {{GuildFeature::ANIMATED_ICON, "ANIMATED_ICON"},
     {GuildFeature::BANNER, "BANNER"},
     {GuildFeature::COMMERCE, "COMMERCE"},
     {GuildFeature::COMMUNITY, "COMMUNITY"},
     {GuildFeature::DISCOVERABLE, "DISCOVERABLE"},
     {GuildFeature::FEATURABLE, "FEATURABLE"},
     {GuildFeature::INVITE_SPLASH, "INVITE_SPLASH"},
     {GuildFeature::MEMBER_VERIFICATION_GATE_ENABLED,
      "MEMBER_VERIFICATION_GATE_ENABLED"},
     {GuildFeature::MONETIZATION_ENABLED, "MONETIZATION_ENABLED"},
     {GuildFeature::MORE_STICKERS, "MORE_STICKERS"},
     {GuildFeature::NEWS, "NEWS"},
     {GuildFeature::PARTNERED, "PARTNERED"},
     {GuildFeature::PREVIEW_ENABLED, "PREVIEW_ENABLED"},
     {GuildFeature::PRIVATE_THREADS, "PRIVATE_THREADS"},
     {GuildFeature::ROLE_ICONS, "ROLE_ICONS"},
     {GuildFeature::SEVEN_DAY_THREAD_ARCHIVE, "SEVEN_DAY_THREAD_ARCHIVE"},
     {GuildFeature::THREE_DAY_THREAD_ARCHIVE, "THREE_DAY_THREAD_ARCHIVE"},
     {GuildFeature::TICKETED_EVENTS_ENABLED, "TICKETED_EVENTS_ENABLED"},
     {GuildFeature::VANITY_URL, "VANITY_URL"},
     {GuildFeature::VERIFIED, "VERIFIED"},
     {GuildFeature::VIP_REGIONS, "VIP_REGIONS"},
     {GuildFeature::WELCOME_SCREEN_ENABLED, "WELCOME_SCREEN_ENABLED"}})

enum class GuildNSFWLevel : int {
    DEFAULT = 0,
    EXPLICIT = 1,
    SAFE = 2,
    AGE_RESTRICTED = 3
};

enum class IntegrationExpireBehavior : int { RemoveRole = 0, Kick = 1 };

enum class InteractionCallbackType : int {
    PONG = 1,
    CHANNEL_MESSAGE_WITH_SOURCE = 4,
    DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE = 5,
    DEFERRED_UPDATE_MESSAGE = 6,
    UPDATE_MESSAGE = 7
};

enum class InteractionCallbackDataFlags : int { EPHEMERAL = 1 << 6 };

enum class InteractionType : int {
    PING = 1,
    APPLICATION_COMMAND = 2,
    MESSAGE_COMPONENT = 3
};

enum class InviteTargetType : int { STREAM = 1, EMBEDDED_APPLICATION = 2 };

enum class MessageActivityType : int {
    JOIN = 1,
    SPECTATE = 2,
    LISTEN = 3,
    JOIN_REQUEST = 5
};

enum class MessageFlags : int {
    CROSSPOSTED = 1 << 0,
    IS_CROSSPOST = 1 << 1,
    SUPPRESS_EMBEDS = 1 << 2,
    SOURCE_MESSAGE_DELETED = 1 << 3,
    URGENT = 1 << 4,
    HAS_THREAD = 1 << 5,
    EPHEMERAL = 1 << 6,
    LOADING = 1 << 7
};

enum class MessageType : int {
    DEFAULT = 0,
    RECIPIENT_ADD = 1,
    RECIPIENT_REMOVE = 2,
    CALL = 3,
    CHANNEL_NAME_CHANGE = 4,
    CHANNEL_ICON_CHANGE = 5,
    CHANNEL_PINNED_MESSAGE = 6,
    GUILD_MEMBER_JOIN = 7,
    USER_PREMIUM_GUILD_SUBSCRIPTION = 8,
    USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1 = 9,
    USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2 = 10,
    USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3 = 11,
    CHANNEL_FOLLOW_ADD = 12,
    GUILD_DISCOVERY_DISQUALIFIED = 14,
    GUILD_DISCOVERY_REQUALIFIED = 15,
    REPLY = 19,
    APPLICATION_COMMAND = 20
};

enum class MFALevel : int { NONE = 0, ELEVATED = 1 };

enum class PremiumTier : int { NONE = 0, TIER_1 = 1, TIER_2 = 2, TIER_3 = 3 };

enum class PermissionFlags : long long {
    CREATE_INSTANT_INVITE = 1ll << 0,
    KICK_MEMBERS = 1ll << 1,
    BAN_MEMBERS = 1ll << 2,
    ADMINISTRATOR = 1ll << 3,
    MANAGE_CHANNELS = 1ll << 4,
    MANAGE_GUILD = 1ll << 5,
    ADD_REACTIONS = 1ll << 6,
    VIEW_AUDIT_LOG = 1ll << 7,
    PRIORITY_SPEAKER = 1ll << 8,
    STREAM = 1ll << 9,
    VIEW_CHANNEL = 1ll << 10,
    SEND_MESSAGES = 1ll << 11,
    SEND_TTS_MESSAGES = 1ll << 12,
    MANAGE_MESSAGES = 1ll << 13,
    EMBED_LINKS = 1ll << 14,
    ATTACH_FILES = 1ll << 15,
    READ_MESSAGE_HISTORY = 1ll << 16,
    MENTION_EVERYONE = 1ll << 17,
    USE_EXTERNAL_EMOJIS = 1ll << 18,
    VIEW_GUILD_INSIGHTS = 1ll << 19,
    CONNECT = 1ll << 20,
    SPEAK = 1ll << 21,
    MUTE_MEMBERS = 1ll << 22,
    DEAFEN_MEMBERS = 1ll << 23,
    MOVE_MEMBERS = 1ll << 24,
    USE_VAD = 1ll << 25,
    CHANGE_NICKNAME = 1ll << 26,
    MANAGE_NICKNAMES = 1ll << 27,
    MANAGE_ROLES = 1ll << 28,
    MANAGE_WEBHOOKS = 1ll << 29,
    MANAGE_EMOJIS_AND_STICKERS = 1ll << 30,
    USE_APPLICATION_COMMANDS = 1ll << 31,
    REQUEST_TO_SPEAK = 1ll << 32,
    MANAGE_EVENTS = 1ll << 33,
    MANAGE_THREADS = 1ll << 34,
    CREATE_PUBLIC_THREADS = 1ll << 35,
    CREATE_PRIVATE_THREADS = 1ll << 36,
    USE_EXTERNAL_STICKERS = 1ll << 37,
    SEND_MESSAGES_IN_THREADS = 1ll << 38,
    START_EMBEDDED_ACTIVITIES = 1ll << 39
};

enum class SystemChannelFlags : int {
    SUPPRESS_JOIN_NOTIFICATIONS = 1 << 0,
    SUPPRESS_PREMIUM_SUBSCRIPTIONS = 1 << 1,
    SUPPRESS_GUILD_REMINDER_NOTIFICATIONS = 1 << 2
};

enum class VerificationLevel : int {
    NONE = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    VERY_HIGH = 4
};

enum class WidgetStyleOptions { shield, banner1, banner2, banner3, banner4 };
NLOHMANN_JSON_SERIALIZE_ENUM(WidgetStyleOptions,
                             {{WidgetStyleOptions::shield, "shield"},
                              {WidgetStyleOptions::banner1, "banner1"},
                              {WidgetStyleOptions::banner2, "banner2"},
                              {WidgetStyleOptions::banner3, "banner3"},
                              {WidgetStyleOptions::banner4, "banner4"}})

} // namespace discordpp
