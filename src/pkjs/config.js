module.exports = [
    {
        "type": "heading",
        "defaultValue": "Orbit Dash Settings"
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Color"
            },
            {
                "type": "color",
                "label": "Background Color",
                "messageKey": "COLOR_BACKGROUND",
                "defaultValue": "000055"
            },
            {
                "type": "color",
                "label": "Clock Color",
                "messageKey": "COLOR_CLOCK",
                "defaultValue": "FFFFFF"
            },
            {
                "type": "color",
                "label": "Digit Color",
                "messageKey": "COLOR_DIGITS",
                "defaultValue": "000000"
            }
        ]
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Date"
            },
            {
                "type": "toggle",
                "label": "Enable Date",
                "messageKey": "ENABLE_DATE",
                "defaultValue": true
            }
        ]
    },
    {
        "type": "submit",
        "defaultValue": "Save"
    }
]