'use client'

import { useState, useEffect, type FormEvent, type ChangeEvent } from 'react'
import type { User } from '@supabase/supabase-js'
import { createClient } from '@/lib/supabase'
import { CUNY_CAMPUSES } from '@/lib/cuny'
import {
  MessageSquare,
  Calendar,
  MapPin,
  ExternalLink,
  LogIn,
  LogOut,
  Mail,
  School,
  CheckCircle,
  KeyRound,
} from 'lucide-react'
import { format } from 'date-fns'

interface Profile {
  full_name: string | null
  campus: string | null
}

interface Channel {
  id: string
  name: string
  description: string | null
  campus: string | null
  type: 'campus' | 'class' | 'interest'
  created_at: string
}

interface Post {
  id: string
  channel_id: string
  author_id: string
  title: string
  content: string
  resource_url: string | null
  upvotes: number
  created_at: string
  profiles?: Profile | null
}

interface SessionRsvp {
  user_id: string
  status: 'going' | 'maybe'
}

interface StudySession {
  id: string
  channel_id: string
  creator_id: string
  title: string
  description: string | null
  location_type: 'in-person' | 'virtual'
  location_detail: string
  start_time: string
  created_at: string
  profiles?: { full_name: string | null } | null
  session_rsvps?: SessionRsvp[]
}

export default function Dashboard() {
  const supabase = createClient()

  const [currentUser, setCurrentUser] = useState<User | null>(null)

  // Auth & OTP State
  const [authEmail, setAuthEmail] = useState('')
  const [authFullName, setAuthFullName] = useState('')
  const [authCampus, setAuthCampus] = useState(CUNY_CAMPUSES[0])
  const [otpCode, setOtpCode] = useState('')
  const [authLoading, setAuthLoading] = useState(false)
  const [authSent, setAuthSent] = useState(false)

  // Channels & Feed State
  const [channels, setChannels] = useState<Channel[]>([])
  const [activeChannel, setActiveChannel] = useState<Channel | null>(null)
  const [posts, setPosts] = useState<Post[]>([])
  const [sessions, setSessions] = useState<StudySession[]>([])
  const [activeTab, setActiveTab] = useState<'threads' | 'sessions'>('threads')

  // Create Form State
  const [newPostTitle, setNewPostTitle] = useState('')
  const [newPostContent, setNewPostContent] = useState('')
  const [newResourceUrl, setNewResourceUrl] = useState('')

  const [sessionTitle, setSessionTitle] = useState('')
  const [sessionType, setSessionType] = useState<'in-person' | 'virtual'>('in-person')
  const [sessionDetail, setSessionDetail] = useState('')
  const [sessionDate, setSessionDate] = useState('')

  // 1. Listen for Supabase session changes
  useEffect(() => {
    async function checkUser() {
      const { data: { user } } = await supabase.auth.getUser()
      setCurrentUser(user)
    }
    checkUser()

    const { data: listener } = supabase.auth.onAuthStateChange((_event, session) => {
      setCurrentUser(session?.user ?? null)
    })

    return () => {
      listener.subscription.unsubscribe()
    }
  }, [supabase])

  // 2. Fetch Channels
  useEffect(() => {
    let isMounted = true

    async function fetchChannels() {
      const { data } = await supabase.from('channels').select('*')
      if (isMounted && data && data.length > 0) {
        const channelList = data as Channel[]
        setChannels(channelList)
        setActiveChannel((prev) => prev ?? channelList[0])
      }
    }

    fetchChannels()

    return () => {
      isMounted = false
    }
  }, [supabase])

  // 3. Fetch Posts & Sessions
  useEffect(() => {
    if (!activeChannel) return
    const channelId = activeChannel.id
    let isMounted = true

    async function fetchFeed() {
      const [postsRes, sessionsRes] = await Promise.all([
        supabase
          .from('posts')
          .select('*, profiles(full_name, campus)')
          .eq('channel_id', channelId)
          .order('created_at', { ascending: false }),
        supabase
          .from('study_sessions')
          .select('*, profiles(full_name), session_rsvps(user_id, status)')
          .eq('channel_id', channelId)
          .order('start_time', { ascending: true }),
      ])

      if (isMounted) {
        setPosts((postsRes.data as Post[]) || [])
        setSessions((sessionsRes.data as StudySession[]) || [])
      }
    }

    fetchFeed()

    return () => {
      isMounted = false
    }
  }, [activeChannel, supabase])

  async function refreshFeed(channelId: string) {
    const [postsRes, sessionsRes] = await Promise.all([
      supabase
        .from('posts')
        .select('*, profiles(full_name, campus)')
        .eq('channel_id', channelId)
        .order('created_at', { ascending: false }),
      supabase
        .from('study_sessions')
        .select('*, profiles(full_name), session_rsvps(user_id, status)')
        .eq('channel_id', channelId)
        .order('start_time', { ascending: true }),
    ])

    setPosts((postsRes.data as Post[]) || [])
    setSessions((sessionsRes.data as StudySession[]) || [])
  }

  // Clean OTP / Magic Link Request without invalid path errors
  async function handleDirectLogin(e: FormEvent<HTMLFormElement>) {
    e.preventDefault()
    setAuthLoading(true)

    const cleanEmail = authEmail.trim().toLowerCase()
    const cleanName = authFullName.trim() || cleanEmail.split('@')[0]

    const { error } = await supabase.auth.signInWithOtp({
      email: cleanEmail,
      options: {
        data: {
          full_name: cleanName,
          campus: authCampus,
        },
      },
    })

    setAuthLoading(false)

    if (error) {
      alert(`Login error: ${error.message}`)
      return
    }

    setAuthSent(true)
  }

  // Verify 6-digit code directly in-app
  async function handleVerifyOtp(e: FormEvent<HTMLFormElement>) {
    e.preventDefault()
    setAuthLoading(true)

    const cleanEmail = authEmail.trim().toLowerCase()

    const { data, error } = await supabase.auth.verifyOtp({
      email: cleanEmail,
      token: otpCode.trim(),
      type: 'email',
    })

    setAuthLoading(false)

    if (error) {
      alert(`Verification failed: ${error.message}`)
      return
    }

    if (data.user) {
      setCurrentUser(data.user)
      setAuthSent(false)
      setOtpCode('')
    }
  }

  async function handleCreatePost(e: FormEvent<HTMLFormElement>) {
    e.preventDefault()
    if (!currentUser) return alert('Please enter your CUNY email and log in first.')
    if (!activeChannel) return

    await supabase.from('posts').insert({
      channel_id: activeChannel.id,
      author_id: currentUser.id,
      title: newPostTitle,
      content: newPostContent,
      resource_url: newResourceUrl || null,
    })

    setNewPostTitle('')
    setNewPostContent('')
    setNewResourceUrl('')
    refreshFeed(activeChannel.id)
  }

  async function handleCreateSession(e: FormEvent<HTMLFormElement>) {
    e.preventDefault()
    if (!currentUser) return alert('Please enter your CUNY email and log in first.')
    if (!activeChannel) return

    await supabase.from('study_sessions').insert({
      channel_id: activeChannel.id,
      creator_id: currentUser.id,
      title: sessionTitle,
      location_type: sessionType,
      location_detail: sessionDetail,
      start_time: new Date(sessionDate).toISOString(),
    })

    setSessionTitle('')
    setSessionDetail('')
    setSessionDate('')
    refreshFeed(activeChannel.id)
  }

  async function handleSignOut() {
    await supabase.auth.signOut()
    setCurrentUser(null)
    setAuthSent(false)
  }

  return (
    <div className="flex h-screen bg-slate-950 text-slate-100 font-sans">
      {/* 1. Sidebar */}
      <aside className="w-72 border-r border-slate-800 bg-slate-900/60 flex flex-col justify-between p-4 overflow-hidden">
        <div className="flex flex-col flex-1 overflow-hidden">
          <div className="flex items-center gap-2 mb-6">
            <div className="bg-blue-600 p-1.5 rounded-lg font-bold text-white shadow-md shadow-blue-500/20">CV</div>
            <span className="font-bold text-lg tracking-tight text-white">CUNYVerse</span>
          </div>

          <div className="flex-1 overflow-y-auto space-y-4 pr-1">
            <div>
              <div className="text-[11px] font-semibold text-slate-400 uppercase tracking-wider mb-2">
                Class & Campus Channels
              </div>
              <div className="space-y-1">
                {channels.map((ch) => (
                  <button
                    type="button"
                    key={ch.id}
                    onClick={() => setActiveChannel(ch)}
                    className={`w-full text-left px-3 py-1.5 rounded-lg text-sm flex items-center gap-2 transition ${
                      activeChannel?.id === ch.id
                        ? 'bg-blue-600 text-white font-medium shadow-sm'
                        : 'text-slate-400 hover:bg-slate-800/80 hover:text-slate-200'
                    }`}
                  >
                    <span className="text-slate-500 font-mono">#</span>
                    <span className="truncate">{ch.name}</span>
                  </button>
                ))}
              </div>
            </div>
          </div>
        </div>

        {/* Auth / OTP Login Container */}
        <div className="border-t border-slate-800/80 pt-4 mt-2">
          {currentUser ? (
            <div className="bg-slate-950 border border-slate-800 rounded-xl p-3 flex items-center justify-between">
              <div className="truncate pr-2">
                <div className="text-[11px] text-slate-400">Signed in as</div>
                <div className="text-xs font-semibold text-slate-200 truncate">{currentUser.email}</div>
              </div>
              <button
                type="button"
                onClick={handleSignOut}
                className="p-1.5 rounded text-slate-400 hover:text-red-400 hover:bg-slate-800 transition"
                title="Sign Out"
              >
                <LogOut className="w-4 h-4" />
              </button>
            </div>
          ) : (
            <div className="bg-slate-950 border border-slate-800 rounded-xl p-3 space-y-2.5">
              <div className="text-xs font-semibold text-slate-300 flex items-center gap-1.5">
                <LogIn className="w-3.5 h-3.5 text-blue-400" /> CUNY Student Login
              </div>

              {authSent ? (
                <div className="space-y-2.5">
                  <div className="p-2.5 bg-emerald-950/50 border border-emerald-800/70 rounded-lg text-[11px] text-emerald-300 flex items-start gap-1.5">
                    <CheckCircle className="w-3.5 h-3.5 flex-shrink-0 mt-0.5 text-emerald-400" />
                    <span>Check your email for the 6-digit code or link:</span>
                  </div>
                  <form onSubmit={handleVerifyOtp} className="space-y-2">
                    <div className="relative">
                      <input
                        type="text"
                        placeholder="6-digit code (e.g. 123456)"
                        value={otpCode}
                        onChange={(e) => setOtpCode(e.target.value)}
                        className="w-full bg-slate-900 border border-slate-800 rounded px-2.5 py-1.5 text-xs text-slate-100 outline-none focus:border-blue-500 font-mono tracking-wider"
                        required
                      />
                      <KeyRound className="w-3.5 h-3.5 text-slate-500 absolute right-2.5 top-2 pointer-events-none" />
                    </div>
                    <button
                      type="submit"
                      disabled={authLoading}
                      className="w-full bg-emerald-600 hover:bg-emerald-700 disabled:opacity-50 text-white font-medium py-1.5 rounded text-xs transition"
                    >
                      {authLoading ? 'Verifying...' : 'Verify Code & Sign In'}
                    </button>
                    <button
                      type="button"
                      onClick={() => setAuthSent(false)}
                      className="w-full text-slate-400 hover:text-slate-200 text-[11px] text-center"
                    >
                      Use different email
                    </button>
                  </form>
                </div>
              ) : (
                <form onSubmit={handleDirectLogin} className="space-y-2">
                  <input
                    type="text"
                    placeholder="Your Name (e.g. Alex M.)"
                    value={authFullName}
                    onChange={(e) => setAuthFullName(e.target.value)}
                    className="w-full bg-slate-900 border border-slate-800 rounded px-2.5 py-1.5 text-xs text-slate-100 outline-none focus:border-blue-500"
                    required
                  />
                  <div className="relative">
                    <select
                      value={authCampus}
                      onChange={(e) => setAuthCampus(e.target.value)}
                      className="w-full bg-slate-900 border border-slate-800 rounded px-2.5 py-1.5 text-xs text-slate-100 outline-none focus:border-blue-500 appearance-none"
                    >
                      {CUNY_CAMPUSES.map((c) => (
                        <option key={c} value={c}>
                          {c}
                        </option>
                      ))}
                    </select>
                    <School className="w-3.5 h-3.5 text-slate-500 absolute right-2.5 top-2 pointer-events-none" />
                  </div>
                  <div className="relative">
                    <input
                      type="email"
                      placeholder="first.last##@login.cuny.edu"
                      value={authEmail}
                      onChange={(e) => setAuthEmail(e.target.value)}
                      className="w-full bg-slate-900 border border-slate-800 rounded px-2.5 py-1.5 text-xs text-slate-100 outline-none focus:border-blue-500 pr-7"
                      required
                    />
                    <Mail className="w-3.5 h-3.5 text-slate-500 absolute right-2.5 top-2 pointer-events-none" />
                  </div>
                  <button
                    type="submit"
                    disabled={authLoading}
                    className="w-full bg-blue-600 hover:bg-blue-700 disabled:opacity-50 text-white font-medium py-1.5 rounded text-xs transition"
                  >
                    {authLoading ? 'Sending Link...' : 'Send Magic Link / Code'}
                  </button>
                </form>
              )}
            </div>
          )}
        </div>
      </aside>

      {/* 2. Main Content Area */}
      <main className="flex-1 flex flex-col min-w-0">
        <header className="h-14 border-b border-slate-800 px-6 flex items-center justify-between bg-slate-900/30 backdrop-blur-sm">
          <div className="flex items-center gap-2.5 font-semibold">
            <span className="text-white">#{activeChannel?.name || 'select-channel'}</span>
            <span className="text-xs text-slate-400 font-normal border border-slate-800 bg-slate-900 px-2 py-0.5 rounded">
              {activeChannel?.campus || 'All CUNY'}
            </span>
          </div>
          <div className="flex gap-2">
            <button
              type="button"
              onClick={() => setActiveTab('threads')}
              className={`px-3 py-1.5 rounded-lg text-sm flex items-center gap-1.5 transition ${
                activeTab === 'threads' ? 'bg-slate-800 text-white shadow-sm' : 'text-slate-400 hover:text-slate-200'
              }`}
            >
              <MessageSquare className="w-4 h-4" /> Discussion Threads
            </button>
            <button
              type="button"
              onClick={() => setActiveTab('sessions')}
              className={`px-3 py-1.5 rounded-lg text-sm flex items-center gap-1.5 transition ${
                activeTab === 'sessions' ? 'bg-slate-800 text-white shadow-sm' : 'text-slate-400 hover:text-slate-200'
              }`}
            >
              <Calendar className="w-4 h-4" /> Study Sessions
            </button>
          </div>
        </header>

        <div className="flex-1 overflow-y-auto p-6 max-w-4xl w-full mx-auto space-y-6">
          {activeTab === 'threads' ? (
            <>
              {/* Thread Creator Form */}
              <form onSubmit={handleCreatePost} className="bg-slate-900 border border-slate-800 rounded-xl p-4 space-y-3 shadow-lg shadow-black/20">
                <input
                  type="text"
                  placeholder="Post Title (e.g., Midterm 1 Study Guide for CS 211)"
                  value={newPostTitle}
                  onChange={(e) => setNewPostTitle(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded-lg px-3 py-2 text-sm outline-none focus:border-blue-500 text-slate-100"
                  required
                />
                <textarea
                  placeholder="Share notes, questions, or resources with your class..."
                  value={newPostContent}
                  onChange={(e) => setNewPostContent(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-800 rounded-lg px-3 py-2 text-sm outline-none focus:border-blue-500 min-h-[85px] text-slate-100"
                  required
                />
                <div className="flex gap-2">
                  <input
                    type="url"
                    placeholder="Resource link (e.g. Google Docs, GitHub, Notion)"
                    value={newResourceUrl}
                    onChange={(e) => setNewResourceUrl(e.target.value)}
                    className="flex-1 bg-slate-950 border border-slate-800 rounded-lg px-3 py-1.5 text-xs outline-none text-slate-100"
                  />
                  <button type="submit" className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-1.5 rounded-lg text-sm font-medium transition">
                    Post Thread
                  </button>
                </div>
              </form>

              {/* Thread Feed */}
              <div className="space-y-3">
                {posts.map((post) => (
                  <article key={post.id} className="bg-slate-900/70 border border-slate-800 rounded-xl p-4 hover:border-slate-700 transition">
                    <div className="flex items-center gap-2 text-xs text-slate-400 mb-2">
                      <span className="font-semibold text-slate-200">{post.profiles?.full_name || 'Anonymous'}</span>
                      <span>•</span>
                      <span>{post.profiles?.campus || 'CUNY'}</span>
                      <span>•</span>
                      <span>{format(new Date(post.created_at), 'MMM d, h:mm a')}</span>
                    </div>
                    <h3 className="font-semibold text-base mb-1 text-slate-100">{post.title}</h3>
                    <p className="text-sm text-slate-300 whitespace-pre-line mb-3">{post.content}</p>
                    {post.resource_url && (
                      <a
                        href={post.resource_url}
                        target="_blank"
                        rel="noreferrer"
                        className="inline-flex items-center gap-1.5 text-xs text-blue-400 bg-blue-950/40 border border-blue-800 px-2.5 py-1 rounded hover:underline"
                      >
                        <ExternalLink className="w-3.5 h-3.5" /> Attached Study Resource
                      </a>
                    )}
                  </article>
                ))}
              </div>
            </>
          ) : (
            <>
              {/* Study Meetup Creator Form */}
              <form onSubmit={handleCreateSession} className="bg-slate-900 border border-slate-800 rounded-xl p-4 space-y-3 shadow-lg shadow-black/20">
                <h4 className="text-sm font-semibold">Propose a Study Session</h4>
                <div className="grid grid-cols-2 gap-3">
                  <input
                    type="text"
                    placeholder="Topic (e.g. Calculus Quiz 3 Review)"
                    value={sessionTitle}
                    onChange={(e) => setSessionTitle(e.target.value)}
                    className="bg-slate-950 border border-slate-800 rounded-lg px-3 py-1.5 text-sm outline-none text-slate-100"
                    required
                  />
                  <input
                    type="datetime-local"
                    value={sessionDate}
                    onChange={(e) => setSessionDate(e.target.value)}
                    className="bg-slate-950 border border-slate-800 rounded-lg px-3 py-1.5 text-sm outline-none text-slate-300"
                    required
                  />
                </div>
                <div className="grid grid-cols-2 gap-3">
                  <select
                    value={sessionType}
                    onChange={(e: ChangeEvent<HTMLSelectElement>) => setSessionType(e.target.value as 'in-person' | 'virtual')}
                    className="bg-slate-950 border border-slate-800 rounded-lg px-3 py-1.5 text-sm outline-none text-slate-100"
                  >
                    <option value="in-person">In-Person (Campus Library / Room)</option>
                    <option value="virtual">Virtual (Zoom / Discord)</option>
                  </select>
                  <input
                    type="text"
                    placeholder={sessionType === 'in-person' ? 'Baruch Library 2nd Fl' : 'https://zoom.us/j/...'}
                    value={sessionDetail}
                    onChange={(e) => setSessionDetail(e.target.value)}
                    className="bg-slate-950 border border-slate-800 rounded-lg px-3 py-1.5 text-sm outline-none text-slate-100"
                    required
                  />
                </div>
                <button type="submit" className="w-full bg-blue-600 hover:bg-blue-700 text-white py-2 rounded-lg text-sm font-medium transition">
                  Schedule Meetup
                </button>
              </form>

              {/* Study Sessions Grid */}
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                {sessions.map((session) => (
                  <div key={session.id} className="bg-slate-900 border border-slate-800 rounded-xl p-4 flex flex-col justify-between">
                    <div>
                      <div className="flex items-center justify-between text-xs text-blue-400 font-semibold mb-1">
                        <span>{format(new Date(session.start_time), 'EEE, MMM d • h:mm a')}</span>
                        <span className="uppercase text-[10px] bg-slate-800 px-1.5 py-0.5 rounded text-slate-300">
                          {session.location_type}
                        </span>
                      </div>
                      <h4 className="font-semibold text-base mb-1">{session.title}</h4>
                      <p className="text-xs text-slate-400 flex items-center gap-1 mb-3">
                        <MapPin className="w-3.5 h-3.5 text-slate-500" />
                        {session.location_detail}
                      </p>
                    </div>
                    <div className="border-t border-slate-800/80 pt-3 flex items-center justify-between">
                      <span className="text-xs text-slate-400">
                        {session.session_rsvps?.length || 0} students going
                      </span>
                      <button type="button" className="bg-slate-800 hover:bg-blue-600 text-slate-200 hover:text-white px-3 py-1 rounded text-xs transition">
                        RSVP
                      </button>
                    </div>
                  </div>
                ))}
              </div>
            </>
          )}
        </div>
      </main>
    </div>
  )
}