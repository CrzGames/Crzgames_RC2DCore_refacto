import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  base: "/", // The base URL your site will be deployed at. You will need to set this if you plan to deploy your site under a sub path, for example GitHub pages.
  srcDir: './src' , // The directory where VitePress will resolve your markdown files from.
  lastUpdated: true,
  sitemap: { // Generate a sitemap for better SEO
    hostname: 'https://librc2d.crzcommon2.com'
  },
  title: "RC2D",
  locales: {
    // Config for Language = EN (default language)
    root: {
      description: "Game Framework",
      label: 'English',
      lang: 'en',
      themeConfig: {
        nav: [
          { text: 'Home', link: '/' },
          { text: 'Guides', link: '/guides/getting-started/prerequisites' },
          { text: 'Platforms', link: '/platforms/introduction' },
          { text: 'API', link: '/api/rc2d' },
          { text: 'Release Notes', link: 'https://github.com/corentin35000/Crzgames_GameEngine/releases' },
        ],
        sidebar: {
          '/api/': [
            {
              text: 'RC2DCore API Reference',
              items: [
                { text: 'rc2d', link: '/api/rc2d' },
                { text: 'rc2d_audio', link: '/api/rc2d_audio' },
                { text: 'rc2d_collision', link: '/api/rc2d_collision' },
                { text: 'rc2d_event', link: '/api/rc2d_event' },
                { text: 'rc2d_filesystem', link: '/api/rc2d_filesystem' },
                { text: 'rc2d_font', link: '/api/rc2d_font' },
                { text: 'rc2d_graphics', link: '/api/rc2d_graphics' },
                { text: 'rc2d_image', link: '/api/rc2d_image' },
                { text: 'rc2d_joystick', link: '/api/rc2d_joystick' },
                { text: 'rc2d_keyboard', link: '/api/rc2d_keyboard' },
                { text: 'rc2d_logger', link: '/api/rc2d_logger' },
                { text: 'rc2d_math', link: '/api/rc2d_math' },
                { text: 'rc2d_mouse', link: '/api/rc2d_mouse' },
                { text: 'rc2d_net', link: '/api/rc2d_net' },
                { text: 'rc2d_rres', link: '/api/rc2d_rres' },
                { text: 'rc2d_spine', link: '/api/rc2d_spine' },
                { text: 'rc2d_system', link: '/api/rc2d_system' },
                { text: 'rc2d_timer', link: '/api/rc2d_timer' },
                { text: 'rc2d_tweening', link: '/api/rc2d_tweening' },
                { text: 'rc2d_window', link: '/api/rc2d_window' },
              ]
            },
          ],
          '/guides/': [
            {
              text: 'Guides',
              items: [
                { 
                  text: 'Getting Started', 
                  items: [
                    { text: 'Prerequisites', link: '/guides/getting-started/prerequisites' },
                    { text: 'Quick Start', link: '/guides/getting-started/quick-start' }
                  ] 
                },
                { 
                  text: 'Development', 
                  items: [
                    { text: 'Cycle Development', link: '/guides/development/development-cycle' },
                    { text: 'Updating Dependencies', link: '/guides/development/updating-dependencies' },
                    { text: 'RC Ecosystem', link: '/guides/development/rc-ecosystem' },
                  ] 
                },
                { text: 'Assets', link: '/guides/assets' },
                { text: 'Icons', link: '/guides/icons' },
                { text: 'Debugging', link: '/guides/debugging' },
                { text: 'Testing', link: '/guides/testing' },
                { text: 'Distribution', link: '/guides/distribution' },
              ]
            },
          ],
          '/platforms/': [
            {
              text: 'Platforms',
              items: [
                { text: 'Introduction', link: '/platforms/introduction' },
                { 
                  text: 'Android', 
                  items: [
                    { text: 'Google Guides', link: '/platforms/android/guides-google' },
                    { text: 'Play Store', link: '/platforms/android/playstore' },
                    { text: 'Distribution', link: '/platforms/android/distribution' },
                    { text: 'Specificity Android', link: '/platforms/android/specificity' },
                    { text: 'Project Android', link: '/platforms/android/project-android' },
                    { text: 'Preparing Project Android', link: '/platforms/android/preparing' },
                  ] 
                },
                { 
                  text: 'iOS', 
                  items: [
                    { text: 'Apple Guides', link: '/platforms/ios/guides-apple' },
                    { text: 'Apple Store', link: '/platforms/ios/applestore' },
                    { text: 'Distribution', link: '/platforms/ios/distribution' },
                    { text: 'Specificity iOS', link: '/platforms/ios/specificity' },
                    { text: 'Project iOS', link: '/platforms/ios/project-ios' },
                    { text: 'Preparing Project iOS', link: '/platforms/ios/preparing' },
                  ]
                },
                { 
                  text: 'macOS', 
                  items: [
                    { text: 'Apple Guides', link: '/platforms/macos/guides-apple' },
                    { text: 'Distribution', link: '/platforms/macos/distribution' },
                    { text: 'Specificity macOS', link: '/platforms/macos/specificity' },
                    { text: 'Project macOS', link: '/platforms/macos/project-macos' },
                    { text: 'Preparing Project macOS', link: '/platforms/macos/preparing' },
                  ] 
                },
                {
                  text: 'Windows',
                  items: [
                    { text: 'Distribution', link: '/platforms/windows/distribution' },
                    { text: 'Project Windows', link: '/platforms/windows/project-windows' },
                    { text: 'Preparing Project Windows', link: '/platforms/windows/preparing' },
                  ]
                },
                { 
                  text: 'Linux', 
                  items: [
                    { text: 'Distribution', link: '/platforms/linux/distribution' },
                    { text: 'Project Linux', link: '/platforms/linux/project-linux' },
                  ] 
                }
              ]
            },
          ],
        }
      }
    },



    // Config for Language = FR
    fr: {
      description: "Framework de jeu",
      label: 'Français',
      lang: 'fr',
      link: '/fr/',
      themeConfig: {
        nav: [
          { text: 'Accueil', link: '/fr/' },
          { text: 'Guides', link: '/fr/guides/getting-started/prerequisites' },
          { text: 'Projets', link: '/fr/projects/android' },
          { text: 'API', link: '/fr/api/rc2d' },
          { text: 'Notes de version', link: 'https://github.com/corentin35000/Crzgames_GameEngine/releases' },
        ],
        sidebar: {
          '/fr/api/': [
            {
              text: 'Référence de l\'API RC2DCore',
              items: [
                { text: 'rc2d', link: '/fr/api/rc2d' },
                { text: 'rc2d_audio', link: '/fr/api/rc2d_audio' },
                { text: 'rc2d_collision', link: '/fr/api/rc2d_collision' },
                { text: 'rc2d_event', link: '/fr/api/rc2d_event' },
                { text: 'rc2d_filesystem', link: '/fr/api/rc2d_filesystem' },
                { text: 'rc2d_font', link: '/fr/api/rc2d_font' },
                { text: 'rc2d_graphics', link: '/fr/api/rc2d_graphics' },
                { text: 'rc2d_image', link: '/fr/api/rc2d_image' },
                { text: 'rc2d_joystick', link: '/fr/api/rc2d_joystick' },
                { text: 'rc2d_keyboard', link: '/fr/api/rc2d_keyboard' },
                { text: 'rc2d_logger', link: '/fr/api/rc2d_logger' },
                { text: 'rc2d_math', link: '/fr/api/rc2d_math' },
                { text: 'rc2d_mouse', link: '/fr/api/rc2d_mouse' },
                { text: 'rc2d_net', link: '/fr/api/rc2d_net' },
                { text: 'rc2d_rres', link: '/fr/api/rc2d_rres' },
                { text: 'rc2d_spine', link: '/fr/api/rc2d_spine' },
                { text: 'rc2d_system', link: '/fr/api/rc2d_system' },
                { text: 'rc2d_timer', link: '/fr/api/rc2d_timer' },
                { text: 'rc2d_tweening', link: '/fr/api/rc2d_tweening' },
                { text: 'rc2d_window', link: '/fr/api/rc2d_window' },
              ]
            },
          ],
          '/fr/guides/': [
            {
              text: 'Guides',
              items: [
                { 
                  text: 'Commencer', 
                  items: [
                    { text: 'Conditions préalables', link: '/fr/guides/getting-started/prerequisites' },
                    { text: 'Démarrage rapide', link: '/fr/guides/getting-started/quick-start' }
                  ] 
                },
                { 
                  text: 'Développement', 
                  items: [
                    { text: 'Cycle de développement', link: '/fr/guides/development/development-cycle' },
                    { text: 'Dépendances', link: '/fr/guides/development/dependencies' }
                  ] 
                },
                { text: 'Assets', link: '/fr/guides/assets' },
                { text: 'Icônes', link: '/fr/guides/icons' },
                { text: 'Débogage', link: '/fr/guides/debugging' },
                { text: 'Test Unitaire', link: '/fr/guides/testing' },
                { text: 'Distribution', link: '/fr/guides/distribution' },
              ]
            },
          ],
          '/fr/platforms/': [
            {
              text: 'Platforms',
              items: [
                { 
                  text: 'Android', 
                  items: [
                    { text: 'Guides Google', link: '/fr/platforms/android/guides-google' },
                    { text: 'Playstore', link: '/fr/platforms/android/playstore' },
                    { text: 'New Release Android', link: '/fr/platforms/android/new-release-android' },
                    { text: 'Sign APK/AAB', link: '/fr/platforms/android/sign' },
                    { text: 'Specificity Android', link: '/fr/platforms/android/specificity' },
                    { text: 'Project Android', link: '/fr/platforms/android/project-android' },
                    { text: 'Must do', link: '/fr/platforms/android/must-do' },
                  ] 
                },
                { text: 'Android', link: '/fr/projects/guides-google' },
                { text: 'iOS', link: '/fr/projects/ios' },
                { text: 'Windows', link: '/fr/projects/windows' },
                { text: 'macOS', link: '/fr/projects/macos' },
                { text: 'Linux', link: '/fr/projects/linux' },
              ]
            },
          ],
        }
      }
    },
  },


  
  // Config for ALL LANGUAGES
  themeConfig: {
    search: {
      provider: 'local'
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/corentin35000/Crzgames_GameEngine' }
    ],
    footer: {
      copyright: 'Copyright © 2024, Crzgames'
    }
  }
})