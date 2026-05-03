// @ts-check

const config = {
  title: "Zoho OS Docs",
  tagline: "Architecture and boot sequence documentation for Zoho OS",
  url: "https://example.com",
  baseUrl: "/",
  organizationName: "zoho-os",
  projectName: "zoho-os-docs",
  onBrokenLinks: "warn",
  markdown: {
    hooks: {
      onBrokenMarkdownLinks: "warn"
    }
  },
  i18n: {
    defaultLocale: "en",
    locales: ["en"]
  },
  presets: [
    [
      "classic",
      {
        docs: {
          routeBasePath: "/",
          sidebarPath: require.resolve("./sidebars.js")
        },
        blog: false,
        pages: false,
        theme: {
          customCss: require.resolve("./src/css/custom.css")
        }
      }
    ]
  ],
  themeConfig: {
    colorMode: {
      defaultMode: "dark",
      disableSwitch: false,
      respectPrefersColorScheme: true
    },
    navbar: {
      title: "Zoho OS Docs",
      hideOnScroll: true,
      items: [
        {
          type: "docSidebar",
          sidebarId: "tutorialSidebar",
          position: "left",
          label: "Boot Sequence"
        },
        {
          to: "/",
          label: "Overview",
          position: "right"
        }
      ]
    },
    footer: {
      style: "dark",
      links: [
        {
          title: "Core Docs",
          items: [
            {
              label: "Boot Sequence",
              to: "/boot/00-overview"
            }
          ]
        }
      ],
      copyright: `Copyright ${new Date().getFullYear()} Zoho OS`
    }
  }
};

module.exports = config;
